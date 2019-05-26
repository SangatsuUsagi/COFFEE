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

#ifndef UTIL_MODELBASE_H
#define UTIL_MODELBASE_H

#include <cstdint>
#include <cfloat>
#include <vector>
#include <array>

#include "util_vector.hpp"

// Material data
struct Material {
    std::array<float, 4> faceColor;
    std::array<float, 3> ambientColor;  // GL_AMBIENT
    std::array<float, 3> diffuseColor;  // GL_DIFFUSE
    std::array<float, 3> specularColor; // GL_SPECULAR
    std::array<float, 3> emissiveColor; // GL_EMISSION
    float power;                        // GL_SHINESS
};

// VBO formats
enum class vboFormat {
    INTERLEAVE = 0,
    SEPARATE,
    BLOCK
};

class baseModel {

    protected:
        // Data type
        vboFormat type;

        // Conversion status
        bool status;

        // Texture file name
        // TODO: 2 or more texture support
        char texFile[256] = "default.tga";

        // Material data
        // TODO: 2 or more material support
        std::vector<Material>  material;

        // Vertex index
        std::vector<std::int_fast32_t> faces;

        // Interleave format data
        std::vector<packedVertex> packedModel;

        // Blocked format data
        std::vector<float> blockedModel;

        // Separate data
        std::vector<vec3> vertices;
        std::vector<vec3> normals;
        std::vector<vec2> textureCoords;

    public:
        baseModel() {
            type = vboFormat::INTERLEAVE;
            status = false;
        }

        baseModel(vboFormat type) : type(type) {
            status = false;
        }

        ~baseModel() {}

        // Return status
        bool getStatus() { return status; }

        // model loader
        virtual void loadModel(const char *fn) = 0;

        // Return texture filename
        char* getTextureFilename()  { return texFile; }

        // Return unified data
        packedVertex*      getPackedVertices()                    { return &packedModel[0]; }
        packedVertex*      getPackedVertices(std::int_fast32_t n) { return &packedModel[n]; }
        std::int_fast32_t  getPackedVerticesSize()                { return packedModel.size(); }

        float*             getBlockedVertices()                    { return &blockedModel[0]; }
        float*             getBlockedVertices(std::int_fast32_t n) { return &blockedModel[n]; }
        std::int_fast32_t  getBlockedVerticesSize()                { return blockedModel.size(); }

        // Return index for unified data
        std::int_fast32_t* getFaces()                       { return &faces[0]; }
        std::int_fast32_t* getFaces(std::int_fast32_t n)    { return &faces[n]; }
        std::int_fast32_t  getFaceSize()                    { return faces.size(); }

        // Return separate data
        vec3*              getVertices()                     { return &vertices[0]; }
        vec3*              getVertices(std::int_fast32_t n)  { return &vertices[n]; }
        std::int_fast32_t  getVerticesSize()                 { return vertices.size(); }
        vec3*              getNormals()                      { return &normals[0]; }
        vec3*              getNormals(std::int_fast32_t n)   { return &normals[n]; }
        std::int_fast32_t  getNormalsSize()                  { return normals.size(); }
        vec2*              getTexCoords()                    { return &textureCoords[0]; }
        vec2*              getTexCoords(std::int_fast32_t n) { return &textureCoords[n]; }
        std::int_fast32_t  getTexCoordsSize()                { return textureCoords.size(); }

        // Get parameters to normalize model size and position
        void getNormalizeParams(float& scale, vec3& trans) {

            vec3 vmax, vmin, vavg;

            vmax.x = vmax.y = vmax.z = FLT_MIN;
            vmin.x = vmin.y = vmin.z = FLT_MAX;
            vavg.x = vavg.y = vavg.z = 0.0f;

            // TODO: Imprement blocked and separate format
            for (packedVertex pv : packedModel) {
                if (pv.vPosition.x > vmax.x) vmax.x = pv.vPosition.x;
                if (pv.vPosition.y > vmax.y) vmax.y = pv.vPosition.y;
                if (pv.vPosition.z > vmax.z) vmax.z = pv.vPosition.z;
                if (pv.vPosition.x < vmin.x) vmin.x = pv.vPosition.x;
                if (pv.vPosition.y < vmin.y) vmin.y = pv.vPosition.y;
                if (pv.vPosition.z < vmin.z) vmin.z = pv.vPosition.z;
                vavg = vavg + pv.vPosition;
            }

            // Scale
            vec3 r = vmax - vmin;
            // TODO : Need to fix to get aspect ratio from window property
            scale = 1.8f / std::max(r.y * 0.5625f, std::max(r.x, r.z));
            std::cout << "Scale        : ";
            std::cout << scale << std::endl;

            // Trans
            vavg = vavg / packedModel.size();
            trans = vavg * scale;
            trans = trans * -1.0f;
            std::cout << "Trans offset : ";
            std::cout << vavg.x << ", " << vavg.y << ", " << vavg.z << std::endl;
        }
};
#endif
