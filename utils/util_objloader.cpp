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

#include <algorithm>
#include <cstdint>      // std::int_fast32_t
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cfloat>
#include <cmath>

#include "util_objloader.hpp"

// Very very simple OBJ model loader...

void objLoader::loadModel(const char *ofn)
{
    // Initialize state
    status = false;

    // Temporary variables
    std::vector<vec3>  t_normals;
    std::vector<vec2>  t_texCoords;
    std::vector<std::int_fast32_t>  vIndex, tIndex, nIndex;

    bool is_norm = false;
    bool is_tex  = false;

    // Open .obj file
    std::ifstream ifs(ofn, std::ios::in);

    // Check status
    if (ifs.fail()) {
        std::cerr << "Error while opening file : " << ofn << std::endl;
        return;
    }

    while (!ifs.eof()) {
        std::string str;
        std::stringstream ss;

        // Read line
        getline(ifs, str, '\n');

        // Skip line with only white spaces
        if (str.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        if (str[0] == 'v' && str[1] == ' ') {
            //  List of geometric vertices, with (x, y, z [,w]) coordinates
            //  w is optional and defaults to 1.0.
            str.erase(str.begin(), str.begin()+1);
            ss.write(str.c_str(), str.length());
            vec3 vert;
            ss >> vert.x >> vert.y >> vert.z;
            vertices.push_back(vert);
        } else if (str[0] == 'v' && str[1] == 't') {
            // List of texture coordinates, in (u, [v ,w]) coordinates
            // these will vary between 0 and 1
            // v and w are optional and default to 0
            is_tex = true;
            str.erase(str.begin(), str.begin()+2);
            ss.write(str.c_str(), str.length());
            vec2 tc;
            ss >> tc.u >> tc.v;
            t_texCoords.push_back(tc);

        } else if (str[0] == 'v' && str[1] == 'n') {
            // List of vertex normals in (x,y,z) form;
            // normals might not be unit vectors.
            is_norm = true;
            str.erase(str.begin(), str.begin()+2);
            ss.write(str.c_str(), str.length());
            vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            t_normals.push_back(norm);

        } else if (str[0] == 'f' && str[1] == ' ') {
            if (is_norm == false && is_tex == false) {
                // Vertex indices
                str.erase(str.begin(), str.begin()+1);
                ss.write(str.c_str(), str.length());
                std::int_fast32_t v0, v1, v2;
                ss >> v0 >> v1 >> v2;
                vIndex.push_back(v0); vIndex.push_back(v1); vIndex.push_back(v2);
            } else if (is_norm == false && is_tex == true) {
                // Vertex texture coordinate indices
                std::replace(str.begin(), str.end(), '/', ' ');
                str.erase(str.begin(), str.begin()+2);
                ss.write(str.c_str(), str.length());
                std::int_fast32_t v0, v1, v2;
                std::int_fast32_t t0, t1, t2;
                ss >> v0 >> t0 >> v1 >> t1 >> v2 >> t2;
                vIndex.push_back(v0); vIndex.push_back(v1); vIndex.push_back(v2);
                tIndex.push_back(t0); tIndex.push_back(t1); tIndex.push_back(t2);
            } else if (is_norm == true && is_tex == true) {
                // Vertex normal indices
                std::replace(str.begin(), str.end(), '/', ' ');
                str.erase(str.begin(), str.begin()+2);
                ss.write(str.c_str(), str.length());
                std::int_fast32_t v0, v1, v2;
                std::int_fast32_t t0, t1, t2;
                std::int_fast32_t n0, n1, n2;
                ss >> v0 >> t0 >> n0 >> v1 >> t1 >> n1 >> v2 >> t2 >> n2;
                vIndex.push_back(v0); vIndex.push_back(v1); vIndex.push_back(v2);
                tIndex.push_back(t0); tIndex.push_back(t1); tIndex.push_back(t2);
                nIndex.push_back(n0); nIndex.push_back(n1); nIndex.push_back(n2);
            }
        } else {
            // DEBUG
            std::cout << "DEBUG: " << str << std::endl;
        }
    }

    // Reorder vertice data with indices
    if (type != vboFormat::SEPARATE)
        faces.resize(vIndex.size());
    if (is_norm == true || is_tex == true) {
        if (is_norm == true)
            normals.resize(vertices.size());
        if (is_tex  == true)
            textureCoords.resize(vertices.size());
        for (auto i = 0; i < vIndex.size(); ++i) {
            // obj index starts with '1', therefore it needs an offset
            auto tv = vIndex.at(i) - 1;
            if (type != vboFormat::SEPARATE)
                // Create index with offset
                faces[i] = tv;
            if (is_tex == true) {
                auto tt = tIndex.at(i) - 1;
                textureCoords.at(tv) = t_texCoords.at(tt);
            }
            if (is_norm == true) {
                auto tn = nIndex.at(i) - 1;
                normals.at(tv) = t_normals.at(tn);
            }
        }
    }

    // Deallocate temp vectors
    std::vector<vec3>().swap(t_normals);
    std::vector<vec2>().swap(t_texCoords);
    std::vector<std::int_fast32_t>().swap(vIndex);
    std::vector<std::int_fast32_t>().swap(tIndex);
    std::vector<std::int_fast32_t>().swap(nIndex);

    if (type == vboFormat::INTERLEAVE) {
        // Convert to unified model data (interleaved)
        packedModel.resize(vertices.size());
        for (auto i = 0; i < vertices.size(); ++i) {
            packedModel.at(i).vPosition = vertices.at(i);
            if (is_norm == true)
                packedModel.at(i).vNormal   = normals.at(i);
            if (is_tex == true)
                packedModel.at(i).vTexCoord = textureCoords.at(i);
        }
        // Deallocate separate buffers
        std::vector<vec3>().swap(vertices);
        std::vector<vec3>().swap(normals);
        std::vector<vec2>().swap(textureCoords);
    }

    status = true;

    return;
}
