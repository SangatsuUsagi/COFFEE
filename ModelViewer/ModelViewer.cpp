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
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <string>

#include "sample_util/SampleApplication.h"
#include "util/shader_utils.h"
#include "sample_util/tga_utils.h"
#include "util/system_utils.h"

#include "util_matrix.hpp"
#include "util_objloader.hpp"
#include "util_xloader.hpp"

// 3D model types
enum class modelFormat {
    MODEL_OBJ = 0,
    MODEL_X,
};

class OBJmodelViewer : public SampleApplication
{
    private:
        GLuint mProgram;
        GLuint mVertexBuffer;
        GLuint mIndexBuffer;
        GLuint mTexture;

        GLint  aPosition;
        GLint  aTexCoord;
        GLint  uSampler;
        GLint  uMatProj;

        // For normalize model
        float scale;
        vec3  trans;

        // OBJ model type and file name
        char* modelName;
        modelFormat type;

        // transform matrix for static(background) images
        Mat4x4 matBack;

        // Sphere model parameters
        baseModel* mModel;

        // Animation parameters
        // std::int_fast32_t   mCount = 0;
        float mAngle = 0.0f;

        // Timer for vsync emulation
        Timer *mTimer;
        float  mTime;

    public:
        // Model Loader
        void loadModel() {};

        void usage()
        {
            std::cout << "Usage : OBJmodelViewer objfile" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        OBJmodelViewer(int argc, char **argv)
            : SampleApplication("OBJmodelViewer", argc, argv, 2, 0)
        {
            if (argc < 2)
                usage();
            else {
                modelName = argv[1];
                std::string fn(modelName);
                std::int_fast32_t ext_i = fn.find_last_of(".");
                std::string extname = fn.substr(ext_i,fn.size()-ext_i);
                std::cout << "Ext :" << extname << std::endl;
                if (extname == ".obj") {
                    type = modelFormat::MODEL_OBJ;
                    objLoader* _mModel = new objLoader();
                    _mModel->loadModel(modelName);
                    mModel = std::move(_mModel);
                } else if (extname == ".x") {
                    type = modelFormat::MODEL_X;
                    xLoader* _mModel = new xLoader();
                    _mModel->loadModel(modelName);
                    mModel = std::move(_mModel);
                } else {
                    usage();
                }
            }
            // Get parameters to adjust model size
            mModel->getNormalizeParams(scale, trans);
        }

        bool initialize() override {
            constexpr char kVS[] = R"(
#version 100
uniform mat4 u_m4Proj;
attribute vec4 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
void main() {
    gl_Position = u_m4Proj * a_position;
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

            // Get index for shader variables and enable it
            aPosition = glGetAttribLocation(mProgram, "a_position");
            glEnableVertexAttribArray((GLuint)aPosition);
            aTexCoord = glGetAttribLocation(mProgram, "a_texcoord");
            glEnableVertexAttribArray((GLuint)aTexCoord);

            uMatProj  = glGetUniformLocation(mProgram, "u_m4Proj");
            uSampler  = glGetUniformLocation(mProgram, "s_texture");

            // Initialize matrix
            Mat4x4 matProj = perspectiveMatrix(deg_to_rad(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);
            Mat4x4 matView = lookAtMatrix(0.0f, 3.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

            Mat4x4 matTrans = translateMatrix(trans.x, trans.y, trans.z);
            Mat4x4 matScale = scaleMatrix(scale, scale, scale);

            matBack = matProj * matView * matTrans * matScale;

            // Create and initialize buffer object
            glGenBuffers(1, &mVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(packedVertex)*mModel->getPackedVerticesSize(),
                    mModel->getPackedVertices(),
                    GL_STATIC_DRAW
            );
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &mIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
            glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(std::int_fast32_t)*mModel->getFaceSize(),
                    mModel->getFaces(),
                    GL_STATIC_DRAW
            );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // Check texture format
            std::string texName = mModel->getTextureFilename();
            std::cout << "Open texture : " << texName << std::endl;
            std::string::size_type pos = texName.find(".tga");
            if (pos == std::string::npos) {
                std::cout << "Only TGA format texture is supported." << std::endl;
                return false;
            }

            // Load texture image
            TGAImage img;
            if (!LoadTGAImageFromFile(texName, &img)) {
                return false;
            }

            mTexture = LoadTextureFromTGAImage(img);
            if (!mTexture) {
                return false;
            }

            // Set GL states
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);

            // Clear buffer
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            // Initialize timer
            mTimer = CreateTimer();
            mTime = mTimer->getAbsoluteTime();

            return true;
        }

        void destroy() override {
            glDeleteBuffers(1, &mIndexBuffer);
            glDeleteBuffers(1, &mVertexBuffer);
            glDeleteTextures(1, &mTexture);
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

            // Set IBO/VBO
            GLsizei stride = sizeof(packedVertex);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glVertexAttribPointer(
                    (GLuint)aPosition, 3, GL_FLOAT, GL_FALSE,
                    stride, (const GLvoid*)offsetof(packedVertex, vPosition)
            );
            glVertexAttribPointer(
                    (GLuint)aTexCoord, 2, GL_FLOAT, GL_FALSE,
                    stride, (const GLvoid*)offsetof(packedVertex, vTexCoord)
            );

            // Use the program object
            glUseProgram(mProgram);
            glUniformMatrix4fv(uMatProj,1, 0, &matMVP(0));

            // Select and bind texture
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(uSampler, 0);
            glBindTexture(GL_TEXTURE_2D, mTexture);

            // Bind index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

            // Draw elements
            glDrawElements(
                    GL_TRIANGLES,           // mode
                    mModel->getFaceSize(),  // count
                    GL_UNSIGNED_INT,        // type
                    (const GLvoid *)0       // elemnt array buffer offset
            );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
    OBJmodelViewer app(argc, argv);
    return app.run();
}
