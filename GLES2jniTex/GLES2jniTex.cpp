//
// Copyright (c) 2019 Tatsuya Kobayashi
// All rights reserved.
//

// Based on gles3jni.cpp from
// https://github.com/googlesamples/android-ndk/

// Copyright 2013 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <iomanip>

#include "sample_util/SampleApplication.h"
#include "util/shader_utils.h"
#include "sample_util/tga_utils.h"
#include "util/system_utils.h"
#include "util/random_utils.h"

#include "GLES2jniTex.hpp"

class GLES2jniTex : public SampleApplication
{
    private:
        GLuint mProgram;
        GLuint mBufferID;
        GLuint mTextureID[MAX_TEXTURES];

        GLint aPos;
        GLint aTexCoord;
        GLint uSampler;
        GLint uScaleRot;
        GLint uOffset;

        GLfloat mOffsets[2*MAX_INSTANCES];
        GLfloat mScaleRot[4*MAX_INSTANCES];
        int mNumInstances = 0;

        GLfloat mScale[2] = {0};
        GLfloat mAngularVelocity[MAX_INSTANCES] ={0};
        float mLastFrameNs = 0;
        GLfloat mAngles[MAX_INSTANCES] = {0};

        // Platform indipendent timer
        Timer *mTimer;

        const struct packedVertex QUAD[4] = {
            // Square with diagonal < 2 so that it fits in a [-1 .. 1]^2 square
            // regardless of rotation.
            {{-0.70711f, -0.70711f}, {0x00, 0xff, 0x00, 0xff}, {0.0f, 0.0f}},
            {{ 0.70711f, -0.70711f}, {0x00, 0x00, 0xff, 0xff}, {1.0f, 0.0f}},
            {{-0.70711f,  0.70711f}, {0xff, 0x00, 0x00, 0xff}, {0.0f, 1.0f}},
            {{ 0.70711f,  0.70711f}, {0xff, 0xff, 0xff, 0xff}, {1.0f, 1.0f}},
        };

    public:
        GLES2jniTex(int argc, char **argv)
            : SampleApplication("GLES2jniTex", argc, argv, 2, 0)
        {}

        void initSceneParams(int w, int h)
        {

            // number of cells along the larger screen dimension
            const float NCELLS_MAJOR = MAX_INSTANCES_PER_SIDE;
            // cell size in scene space
            const float CELL_SIZE = 2.0f / NCELLS_MAJOR;

            // Calculations are done in "landscape", i.e. assuming dim[0] >= dim[1].
            // Only at the end are values put in the opposite order if h > w.
            float dim[2] = {fmaxf(w,h), fminf(w,h)};
            float aspect[2] = {dim[0] / dim[1], dim[1] / dim[0]};
            float scene2clip[2] = {1.0f, aspect[0]};
            int ncells[2] = {
                (int)(NCELLS_MAJOR),
                (int)floorf(NCELLS_MAJOR * aspect[1])
            };

            float centers[2][MAX_INSTANCES_PER_SIDE];
            for (auto d = 0; d < 2; d++) {
                int offset = -ncells[d] / NCELLS_MAJOR; // -1.0 for d=0
                for (auto i = 0; i < ncells[d]; i++) {
                    centers[d][i] = scene2clip[d] * (CELL_SIZE*(i + 0.5f) + offset);
                }
            }

            int major = w >= h ? 0 : 1;
            int minor = w >= h ? 1 : 0;
            // outer product of centers[0] and centers[1]
            for (auto i = 0; i < ncells[0]; i++) {
                for (auto j = 0; j < ncells[1]; j++) {
                    int idx = i*ncells[1] + j;
                    mOffsets[2*idx + major] = centers[0][i];
                    mOffsets[2*idx + minor] = centers[1][j] - 1.0f;
                }
            }

            mNumInstances = ncells[0] * ncells[1];
            mScale[major] = 0.5f * CELL_SIZE * scene2clip[0];
            mScale[minor] = 0.5f * CELL_SIZE * scene2clip[1];

            angle::RNG mRNG;
            for (auto i = 0; i < mNumInstances; i++) {
                mAngles[i] = mRNG.randomFloatBetween(0.0f, 1.0f) * TWO_PI;
                mAngularVelocity[i] = MAX_ROT_SPEED * (2.0 * mRNG.randomFloatBetween(0.0f, 1.0f) - 1.0);
            }

            return;
        }

        void updateSceneParams()
        {
            float nowNs = mTimer->getAbsoluteTime();

            if (mLastFrameNs > 0) {
                float dt = nowNs - mLastFrameNs;

                for (auto i = 0; i < mNumInstances; i++) {
                    mAngles[i] += mAngularVelocity[i] * dt;
                    if (mAngles[i] >= TWO_PI) {
                        mAngles[i] -= TWO_PI;
                    } else if (mAngles[i] <= -TWO_PI) {
                        mAngles[i] += TWO_PI;
                    }
                }

                for (auto i = 0; i < mNumInstances; i++) {
                    float s = sinf(mAngles[i]);
                    float c = cosf(mAngles[i]);
                    mScaleRot[4*i + 0] =  c * mScale[0];
                    mScaleRot[4*i + 1] =  s * mScale[1];
                    mScaleRot[4*i + 2] = -s * mScale[0];
                    mScaleRot[4*i + 3] =  c * mScale[1];
                }
            }

            mLastFrameNs = nowNs;
        }

        bool initialize() override {
            constexpr char kVS[] = R"(
#version 100
uniform mat2 u_scaleRot;
uniform vec2 u_offset;
attribute vec2 a_pos;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
void main() {
    gl_Position = vec4(u_scaleRot*a_pos + u_offset, 0.0, 1.0);
    v_texcoord = a_texcoord;
}
)";

            constexpr char kFS[] = R"(
#version 100
precision mediump float;
uniform sampler2D s_texture;
varying vec2 v_texcoord;
void main() {
    gl_FragColor = texture2D(s_texture, v_texcoord);
}
)";

            mProgram = CompileProgram(kVS, kFS);
            if (!mProgram) {
                return false;
            }

            // Get index for shader variables
            aPos = glGetAttribLocation(mProgram, "a_pos");
            glEnableVertexAttribArray(aPos);
            aTexCoord = glGetAttribLocation(mProgram, "a_texcoord");
            glEnableVertexAttribArray(aTexCoord);

            uSampler = glGetUniformLocation(mProgram, "s_texture");
            uScaleRot = glGetUniformLocation(mProgram, "u_scaleRot");
            uOffset = glGetUniformLocation(mProgram, "u_offset");

            // Create buffer object
            glGenBuffers(1, &mBufferID);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), &QUAD[0], GL_STATIC_DRAW);

            for (auto i = 0; i < MAX_TEXTURES; ++i) {
                std::stringstream jaketStr;
                jaketStr << angle::GetExecutableDirectory() << "/"
                    << std::setfill('0') << std::setw(3) << i << ".tga";

                TGAImage img;
                if (!LoadTGAImageFromFile(jaketStr.str(), &img)) {
                    return false;
                }
                mTextureID[i] = LoadTextureFromTGAImage(img);
                if (!mTextureID[i]) {
                    return false;
                }
            }

            // Initialize animation parameters
            initSceneParams(getWindow()->getWidth(), getWindow()->getHeight());

            // Set the viewport
            glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

            // Create timer for animation
            mTimer = CreateTimer();

            glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

            return true;
        }

        void destroy() override {
            glDeleteProgram(mProgram);
        }

        void draw() override
        {
            // Update parameters
            updateSceneParams();

            // Clear the color buffer
            glClearColor( 0.2f, 0.2f, 0.3f, 1.0f );
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

            // Use the program object
            glUseProgram(mProgram);

            // Load the vertex data
            glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
            glVertexAttribPointer(
                    aPos, 2, GL_FLOAT, GL_FALSE,
                    sizeof(packedVertex),
                    (const GLvoid*)offsetof(packedVertex, pos)
            );
            glVertexAttribPointer(
                    aTexCoord, 2, GL_FLOAT, GL_FALSE,
                    sizeof(packedVertex),
                    (const GLvoid*)offsetof(packedVertex, tex)
            );

            glActiveTexture(GL_TEXTURE0);
            glUniform1i(uSampler, 0);

            for (auto i = 0; i < mNumInstances; i++) {
                glBindTexture(GL_TEXTURE_2D, mTextureID[i % (MAX_TEXTURES - 1)]);
                glUniformMatrix2fv(uScaleRot, 1, GL_FALSE, mScaleRot + 4 * i);
                glUniform2fv(uOffset, 1, mOffsets + 2 * i);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
};

int main(int argc, char **argv)
{
    GLES2jniTex app(argc, argv);
    return app.run();
}
