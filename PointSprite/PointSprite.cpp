//
// Copyright (c) 2019 Tatsuya Kobayashi
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//  * Neither the name of the author nor the names of contributors may
// be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "sample_util/SampleApplication.h"
#include "util/shader_utils.h"
#include "sample_util/tga_utils.h"
#include "util/system_utils.h"

#include "util_matrix.hpp"
#include "util_modelgen.hpp"

class PointSprite : public SampleApplication
{
    private:
        GLuint mProgram;
        GLuint mVertBuffer;
        GLuint mColorBuffer;
        GLuint mTexture;

        GLint  aPosition;
        GLint  aColor;
        GLint  uSampler;
        GLint  uMatProj;

        // transform matrix for static(background) images
        Mat4x4 matBack;

        // Sphere model parameters
        modelSphere*            mdlSphere;
        static const int        mSphereRow  = 64;
        static const int        mSphereCol  = 64;
        static constexpr float  mSphereRad  = 0.5f;

        // Sphere coordinates
        static const int spherePosSize = mSphereRow * mSphereCol;
        std::array<vec3, spherePosSize>  mPosition;

        // Animation parameters
        int   mCount = 0;
        float mAngle = 0.0f;

        // Timer for vsync emulation
        Timer *mTimer;
        float  mTime;

    public:
        PointSprite(int argc, char **argv)
            : SampleApplication("PointSprite", argc, argv, 2, 0)
        {
        }

        bool initialize() override {
            constexpr char kVS[] = R"(
#version 100
uniform mat4 u_m4Proj;
attribute vec4 a_v4Position;
attribute vec4 a_v4Color;
varying   vec4 v_v4Color;
void main() {
    gl_Position = u_m4Proj * a_v4Position;
    gl_PointSize = 16.0;
    v_v4Color = a_v4Color;
}
)";

            constexpr char kFS[] = R"(
#version 100
precision mediump float;
uniform sampler2D s_texture;
varying vec4 v_v4Color;
void main() {
    vec4 baseColor = texture2D(s_texture, gl_PointCoord);
    if (baseColor.a < 0.5) {
        discard;
    } else {
        gl_FragColor = baseColor * v_v4Color;
    }
}
)";

            mProgram = CompileProgram(kVS, kFS);
            if (!mProgram) {
                return false;
            }

            // Get index for shader variables
            aPosition = glGetAttribLocation(mProgram, "a_v4Position");
            glEnableVertexAttribArray((GLuint)aPosition);
            aColor = glGetAttribLocation(mProgram, "a_v4Color");
            glEnableVertexAttribArray((GLuint)aColor);

            uMatProj  = glGetUniformLocation(mProgram, "u_m4Proj");
            uSampler  = glGetUniformLocation(mProgram, "s_texture");

            // Initialize matrix
            Mat4x4 matProj = orthogonalMatrix(-1.7f, 1.7f, -1.0f, 1.0f, 0.0f, 20.0f);
            Mat4x4 matView = lookAtMatrix(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            matBack = matProj * matView;

            // Initialize model
            mdlSphere = new modelSphere(vboFormat::SEPARATE, mSphereRow, mSphereCol, mSphereRad);

            // Create and initialize buffer object
            glGenBuffers(1, &mVertBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mVertBuffer);
            glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(vec3) * mdlSphere->getVerticesSize(),
                    mdlSphere->getVertices(),
                    GL_STREAM_DRAW
            );

            glGenBuffers(1, &mColorBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
            glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(vec4) * mdlSphere->getVertColorSize(),
                    mdlSphere->getVertColor(),
                    GL_STATIC_DRAW
            );

            // Load texture image
            TGAImage img;
            if (!LoadTGAImageFromFile("smoke_64x64.tga", &img)) {
                return false;
            }
            mTexture = LoadTextureFromTGAImage(img);
            if (!mTexture) {
                return false;
            }

            // Set GL states
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glEnable(GL_BLEND);

            // Clear buffer
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            // Initialize timer
            mTimer = CreateTimer();
            mTime = mTimer->getAbsoluteTime();

            return true;
        }

        void destroy() override {
            // glDeleteTextures(1, mTexture);
            // glDeleteBuffers(1, mVertBuffer);
            glDeleteProgram(mProgram);
        }

        void draw() override
        {
            // Clear the color buffer
            glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update angle parameters
            mAngle = mAngle + 0.01f;

            // Create the rotate and translate model view matrix
            Mat4x4 matRot = rotateYMatrix(mAngle);
            Mat4x4 matMVP = matBack * matRot;

            // Create model parameters based on animation parameters
            mCount += 3;
            float rad = (float)(mCount % 360) * M_PI / 180.0f;

            // Create animated sphere point array
            float t = std::cos(rad) / 2.0f;
            for (int i = 0; i < spherePosSize; ++i) {
                 mPosition[i].x = (mdlSphere->getVertices(i))->x * (1.0f + t);
                 mPosition[i].y = (mdlSphere->getVertices(i))->y * (1.0f + t);
                 mPosition[i].z = (mdlSphere->getVertices(i))->z * (1.0f + t);
            }

            // Use the program object
            glUseProgram(mProgram);
            glUniformMatrix4fv(uMatProj,1, 0, &matMVP(0));

            // Update VBO
            glBindBuffer(GL_ARRAY_BUFFER, mVertBuffer);
            glBufferSubData(
                     GL_ARRAY_BUFFER,
                     0,
                     sizeof(mPosition),
                     &(mPosition[0])
            );

            // Set vertex color buffer
            glVertexAttribPointer((GLuint)aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
            glVertexAttribPointer((GLuint)aColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

            // Select and set texture
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(uSampler, 0);
            glBindTexture(GL_TEXTURE_2D, mTexture);

            // Draw point(s)
            glDrawArrays(GL_POINTS,  0, mSphereCol * mSphereRow);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Vsync emulation
            float cTime = mTimer->getAbsoluteTime();
            float eTime = (cTime - mTime) * 1e3;
            mTime = cTime;
            if (eTime < 16.666f) angle::Sleep((std::int_fast32_t)(16.666f - eTime));
        }
};

int main(int argc, char **argv)
{
    PointSprite app(argc, argv);
    return app.run();
}
