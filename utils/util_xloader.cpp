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

#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "util_xloader.hpp"

// Very very simple X model loader
// This loader doesn't parse any headers in X file.
// And only single material data is supported.

void xLoader::loadModel(const char *xfn)
{
    // Initialize state
    status = false;
    state = X_MESH_HDR;

    // Temporary variables
    std::vector<vec3>  t_normals;
    std::int_fast32_t  t_nFaceNormals;
    std::vector<std::int_fast32_t> t_faceNormals;

    // Open .x file
    std::ifstream ifs(xfn, std::ios::in);

    // Check status
    if (ifs.fail()) {
        std::cerr << "Error while opening file : " << xfn << std::endl;
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

        switch (state) {
            case X_MESH_HDR:
                // template Mesh {
                //  <3D82AB44-62DA-11cf-AB39-0020AF71E433>
                //  DWORD nVertices;
                //  array Vector vertices[nVertices];
                //  DWORD nFaces;
                //  array MeshFace faces[nFaces];
                //  [...]
                // }
                {
                    if (str.find("Mesh {") == 0) {
                        std::cout << "Found Mesh entry" << std::endl;
                        state = X_VERTICE_CNT;
                    }
                }
                break;

            case X_VERTICE_CNT:
                {
                    std::int_fast32_t nVertices = 0;
                    ss.write(str.c_str(), str.length());;
                    ss >> nVertices;
                    if (nVertices != 0) {
                        std::cout << "nVertices : " << nVertices << std::endl;
                        count = nVertices;
                        state = X_VERTICE_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_VERTICE_DAT:
                //  Example
                //  5748;
                //  0.38836;0.50363;-0.11262;,
                {
                    std::replace(str.begin(), str.end(), ';', ' ');
                    ss.write(str.c_str(), str.length());;
                    vec3 vert;
                    ss >> vert.x >> vert.y >> vert.z;
                    vertices.push_back(vert);
                    if (--count == 0) {
                        std::cout << "#" << vertices.size() << " vertices" << std::endl;
                        state = X_FACE_CNT;
                    }
                }
                break;

            case X_FACE_CNT:
                //  Example
                //  5008;
                //  4;0,1,2,3;,
                //  TODO: Need to split square to two tris
                {
                    std::int_fast32_t nFaces = 0;
                    ss.write(str.c_str(), str.length());;
                    ss >> nFaces;
                    if (nFaces != 0) {
                        std::cout << "nFaces: " << nFaces << std::endl;
                        count = nFaces;
                        state = X_FACE_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_FACE_DAT:
                {
                    std::replace(str.begin(), str.end(), ';', ',');
                    std::replace(str.begin(), str.end(), ',', ' ');
                    std::int_fast32_t fn, fa, fb, fc, fd;
                    ss.write(str.c_str(), str.length());;
                    ss >> fn;
                    if (fn == 3) {
                        ss >> fa >> fb >> fc;
                        faces.push_back(fa);
                        faces.push_back(fb);
                        faces.push_back(fc);
                    } else if (fn == 4) {
                        ss >> fa >> fb >> fc >> fd;
                        // Upper triangle
                        faces.push_back(fa);
                        faces.push_back(fb);
                        faces.push_back(fc);
                        // Lower triangle
                        faces.push_back(fa);
                        faces.push_back(fc);
                        faces.push_back(fd);
                    } else {
                        std::cout << "Only triangle or square polygons can be supported." << std::endl;
                        state = X_END;
                    }
                    if (--count == 0) {
                        std::cout << "#" << faces.size() << " faces" << std::endl;
                        state = X_MATERIAL_HDR;
                    }
                }
                break;

            case X_MATERIAL_HDR:
                //  Example
                //  MeshMaterialList {
                //   1;
                //   5008;
                //   0,
                //   0,
                //   }
                {
                    if (str.find("MeshMaterialList") != std::string::npos) {
                        std::cout << "Found MeshMaterialList entry" << std::endl;
                        state = X_MATERIAL_NUM;
                    }
                }
                break;
            case X_MATERIAL_NUM:
                {
                    std::int_fast32_t nMaterials;
                    ss.write(str.c_str(), str.length());;
                    ss >> nMaterials;
                    if (nMaterials != 1) {
                        std::cout << "Only one matrial can be supported." << std::endl;
                        state = X_END;
                    } else
                        state = X_MATERIAL_CNT;
                }
                break;

            case X_MATERIAL_CNT:
                {
                    nFaceIndexes = 0;
                    ss.write(str.c_str(), str.length());;
                    ss >> nFaceIndexes;
                    if (nFaceIndexes != 0) {
                        std::cout << "nFaceIndexes: " << nFaceIndexes << std::endl;
                        count = nFaceIndexes;
                        state = X_MATERIAL_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_MATERIAL_DAT:
                {
                    // std::int_fast32_t faceIndex;
                    // ss.write(str.c_str(), str.length());;
                    // ss >> faceIndex;
                    // faceIndexes.push_back(faceIndex);
                    if (--count == 0) {
                        // std::cout << "#" << faceIndexes.size() << " face indexes" << std::endl;
                        state = X_MATERIAL0_HDR;
                    }
                }
                break;

            case X_MATERIAL0_HDR:
                // template Material {
                //  <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
                //  ColorRGBA faceColor;
                //  FLOAT power;
                //  ColorRGB specularColor;
                //  ColorRGB emissiveColor;
                //  [...]
                // }
                {
                    if (str.find("Material {") != std::string::npos) {
                        std::cout << "Found Material entry" << std::endl;
                        state = X_MATERIAL0_DAT;
                    }
                }
                break;

            case X_MATERIAL0_DAT:
                {
                    // facecolor
                    std::replace(str.begin(), str.end(), ';', ' ');
                    float mr, mg, mb, ma;
                    ss.write(str.c_str(), str.length());;
                    ss >> mr >> mg >> mb >> ma;
                    faceColor[0] = mr;
                    faceColor[1] = mg;
                    faceColor[2] = mb;
                    faceColor[3] = ma;
                    std::cout << "Face Color :" << std::fixed << mr << "," << mg << "," << mb << "," << ma << std::endl;
                    // power
                    getline(ifs, str, '\n');
                    std::replace(str.begin(), str.end(), ';', ' ');
                    float mp;
                    ss.write(str.c_str(), str.length());;
                    ss >> mp;
                    power = mp;
                    std::cout << "Power :" << std::fixed << mp << std::endl;
                    // specularcolor
                    getline(ifs, str, '\n');
                    std::replace(str.begin(), str.end(), ';', ' ');
                    ss.write(str.c_str(), str.length());;
                    ss >> mr >> mg >> mb;
                    specularColor[0] = mr;
                    specularColor[1] = mg;
                    specularColor[2] = mb;
                    std::cout << "Specular Color :" << std::fixed << mr << "," << mg << "," << mb << std::endl;
                    // emissivecolor
                    getline(ifs, str, '\n');
                    std::replace(str.begin(), str.end(), ';', ' ');
                    ss.write(str.c_str(), str.length());;
                    ss >> mr >> mg >> mb;
                    emissiveColor[0] = mr;
                    emissiveColor[1] = mg;
                    emissiveColor[2] = mb;
                    std::cout << "Emissive Color :" << std::fixed << mr << "," << mg << "," << mb << std::endl;

                    state = X_MATERIAL0_TEX_HDR;
                }
                break;

            case X_MATERIAL0_TEX_HDR:
                // Example
                // template TextureFilename {
                //  <A42790E1-7810-11cf-8F52-0040333594A3>
                //  STRING filename;
                // }
                {
                    if (str.find("TextureFilename") != std::string::npos) {
                        state = X_MATERIAL0_TEX_NAM;
                    }
                }
                break;

            case X_MATERIAL0_TEX_NAM:
                {
                    std::string::size_type fpos = str.find("\"");
                    std::string::size_type rpos = str.rfind("\"");
                    if ((fpos == std::string::npos) || (rpos == std::string::npos)) {
                        std::cout << "Cannot parse texture filename." << std::endl;
                        state = X_END;
                        break;
                    }
                    if ((rpos - fpos) > 255) {
                        std::cout << "Texture filename is too long." << std::endl;
                        state = X_END;
                        break;
                    }
                    // texFile is initialized with zero, no need to add \0
                    str.copy(texFile, rpos - fpos - 1, fpos + 1);
                    texFile[rpos - fpos] = '\0';
                    std::cout << "Texture Filename: \"" << texFile << "\"" << std::endl;
                    state = X_NORMAL_HDR;
                }
                break;

            case X_NORMAL_HDR:
                // template MeshNormals {
                //  <F6F23F43-7686-11cf-8F52-0040333594A3>
                //  DWORD nNormals;
                //  array Vector normals[nNormals];
                //  DWORD nFaceNormals;
                //  array MeshFace faceNormals[nFaceNormals];
                // }
                {
                    if (str.find("MeshNormals") != std::string::npos) {
                        std::cout << "Found Mesh Normals entry" << std::endl;
                        state = X_NORMAL_V_CNT;
                    }
                }
                break;

            case X_NORMAL_V_CNT:
                {
                    std::int_fast32_t nNormals = 0;
                    ss.write(str.c_str(), str.length());;
                    ss >> nNormals;
                    if (nNormals != 0) {
                        std::cout << "nNormals: " << nNormals << std::endl;
                        count = nNormals;
                        state = X_NORMAL_V_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_NORMAL_V_DAT:
                {
                    std::replace(str.begin(), str.end(), ';', ' ');
                    ss.write(str.c_str(), str.length());;
                    vec3 norm;
                    ss >> norm.x >> norm.y >> norm.z;
                    t_normals.push_back(norm);
                    if (--count == 0) {
                        std::cout << "#" << t_normals.size() << " normals (tmp)" << std::endl;
                        state = X_NORMAL_F_CNT;
                    }
                }
                break;

            case X_NORMAL_F_CNT:
                {
                    t_nFaceNormals = 0;
                    ss.write(str.c_str(), str.length());;
                    ss >> t_nFaceNormals;
                    if (t_nFaceNormals != 0) {
                        std::cout << "nFaceNormals (tmp): " << t_nFaceNormals << std::endl;
                        count = t_nFaceNormals;
                        state = X_NORMAL_F_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_NORMAL_F_DAT:
                {
                    std::replace(str.begin(), str.end(), ';', ',');
                    std::replace(str.begin(), str.end(), ',', ' ');
                    std::int_fast32_t nn, na, nb, nc, nd;
                    ss.write(str.c_str(), str.length());;
                    ss >> nn;
                    if (nn == 3) {
                        ss >> na >> nb >> nc;
                        t_faceNormals.push_back(na);
                        t_faceNormals.push_back(nb);
                        t_faceNormals.push_back(nc);
                    } else if (nn == 4) {
                        ss >> na >> nb >> nc >> nd;
                        // Upper triangle
                        t_faceNormals.push_back(na);
                        t_faceNormals.push_back(nb);
                        t_faceNormals.push_back(nc);
                        // Lower triangle
                        t_faceNormals.push_back(na);
                        t_faceNormals.push_back(nc);
                        t_faceNormals.push_back(nd);
                    } else {
                        std::cout << "Only triangle or square polygons can be supported." << std::endl;
                        state = X_END;
                    }
                    if (--count == 0) {
                        std::cout << "#" << t_faceNormals.size() << " face normals" << std::endl;
                        state = X_UV_HDR;
                    }
                }
                break;

            case X_UV_HDR:
                // template MeshTextureCoords {
                //  <F6F23F40-7686-11cf-8F52-0040333594A3>
                //  DWORD nTextureCoords;
                //  array Coords2d textureCoords[nTextureCoords];
                // }
                {
                    if (str.find("MeshTextureCoords") != std::string::npos) {
                        std::cout << "Found MeshTextureCoords entry" << std::endl;
                        state = X_UV_CNT;
                    }
                }
                break;

            case X_UV_CNT:
                {
                    std::int_fast32_t nTextureCoords = 0;
                    ss.write(str.c_str(), str.length());
                    ss >> nTextureCoords;
                    if (nTextureCoords != 0) {
                        std::cout << "nTextureCoords: " << nTextureCoords << std::endl;
                        count = nTextureCoords;
                        state = X_UV_DAT;
                    } else
                        state = X_END;
                }
                break;

            case X_UV_DAT:
                {
                    std::replace(str.begin(), str.end(), ';', ',');
                    std::replace(str.begin(), str.end(), ',', ' ');
                    ss.write(str.c_str(), str.length());
                    vec2 tc;
                    ss >> tc.u >> tc.v;
                    textureCoords.push_back(tc);
                    if (--count == 0) {
                        std::cout << "#" << textureCoords.size() << " texture coords" << std::endl;
                        state = X_END;
                    }
                }

                status = true;
                break;

            case X_END:
                break;

            default:
                {
                    std::cout << "Unknown state" << std::endl;
                    state = X_END;
                }
        }

        if (state == X_END) break;
    }

    std::cout << "Status: " << std::boolalpha << status << std::endl;

    // Reoder normals
    // Todo :: reorder with std::map
    // normals.resize(nVertices * 3);
    normals.resize(vertices.size());
    for (auto i = 0; i < faces.size(); ++i) {
        int idx_v = faces.at(i);
        int idx_n = t_faceNormals.at(i);

        normals.at(idx_v) = t_normals.at(idx_n);
    }

    // Deallocate temp vectors
    std::vector<vec3>().swap(t_normals);
    std::vector<std::int_fast32_t>().swap(t_faceNormals);

    if (type == vboFormat::INTERLEAVE) {
        // Convert to unified model data (interleaved)
        packedModel.resize(vertices.size());
        for (auto i = 0; i < vertices.size(); ++i) {
            packedModel.at(i).vPosition = vertices.at(i);
            packedModel.at(i).vNormal   = normals.at(i);
            packedModel.at(i).vTexCoord = textureCoords.at(i);
        }
        std::vector<vec3>().swap(vertices);
        std::vector<vec3>().swap(normals);
        std::vector<vec2>().swap(textureCoords);
    }
    return;
}
