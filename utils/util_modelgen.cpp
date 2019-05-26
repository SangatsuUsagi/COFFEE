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
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "util_modelgen.hpp"
#include "util_vector.hpp"

/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 1].
 */
float HueToRGB(float v1, float v2, float vH) {
    if (vH < 0)
        vH += 1;

    if (vH > 1)
        vH -= 1;

    if ((6 * vH) < 1)
        return (v1 + (v2 - v1) * 6 * vH);

    if ((2 * vH) < 1)
        return v2;

    if ((3 * vH) < 2)
        return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

    return v1;
}

vec4 HSLToRGB(float h, float s, float l)
{
    vec4 rgba;

    if (s == 0.0f) {
        rgba.r = rgba.g = rgba.b = l;
    } else {
        float v1, v2;

        v2 = (l < 0.5) ? (l * (1.0f + s)) : ((l + s) - (l * s));
        v1 = 2.0f * l - v2;

        rgba.r = HueToRGB(v1, v2, h + (1.0f / 3.0f));
        rgba.g = HueToRGB(v1, v2, h);
        rgba.b = HueToRGB(v1, v2, h - (1.0f / 3.0f));
    }
    rgba.a = 1.0f;

    return rgba;
}

modelSphere::modelSphere(vboFormat type, std::int_fast32_t row, std::int_fast32_t column, float rad)
    : baseModel(type), row(row), column(column), rad(rad)
{
    if (type != vboFormat::SEPARATE)
        packedModel.resize(column * row);

    // TODO: Add BLOCKED format
    // Create position and normal
    for (auto i = 0; i < row; i++) {
        vec3 tv, tn;
        vec2 tc;
        float r = M_PI / row * (float)i;
        float rr = std::sin(r);
        tn.y = std::cos(r);
        for (auto j = 0; j < column; j++) {
            float tr = M_PI * 2.0f / (float)column * (float)j;
            tv.x = rr * rad * std::cos(tr);
            tv.y = tn.y * rad;
            tv.z = rr * rad * std::sin(tr);
            tn.x = rr * std::cos(tr);
            tn.z = rr * std::sin(tr);
            if ((column != 0) && (j != 0))
                tc.u = 1.0f - 1.0f / (column * j);
            else
                tc.u = 0.0f;
            if ((row != 0) && (i != 0))
                tc.v = 1.0f / (row * i);
            else
                tc.v = 0.0f;
            if (type == vboFormat::INTERLEAVE) {
                packedModel.at(i * column + j).vPosition = tv;
                packedModel.at(i * column + j).vNormal   = tn;
                packedModel.at(i * column + j).vTexCoord = tc;
            } else if (type == vboFormat::SEPARATE) {
                vertices.push_back(tv);
                normals.push_back(tn);
                textureCoords.push_back(tc);
                // vertColor.push_back(hsv(360 / row * i, 1.0f, 1.0f));
                vertColor.push_back(HSLToRGB((float)(i * row + j)/(row * column), 0.5f, 0.5f));
            }
        }
    }

    // Create index
    std::int_fast32_t r = 0;
    faces.resize(row * column * 6);
    for (auto i = 0; i < row; i++) {
        for (auto j = 0; j < column; j++) {
            r = (column + 1) * i + j;
            // Upper triangle
            (faces.at(i * column * 6 + j * 6 + 0)) = r             ;
            (faces.at(i * column * 6 + j * 6 + 1)) = r          + 1;
            (faces.at(i * column * 6 + j * 6 + 2)) = r + column + 2;
            // Lower triangle
            (faces.at(i * column * 6 + j * 6 + 3)) = r             ;
            (faces.at(i * column * 6 + j * 6 + 4)) = r + column + 2;
            (faces.at(i * column * 6 + j * 6 + 5)) = r + column + 1;
        }
    }

    std::cout << "Sphere model has generated"  << std::endl;
    std::cout << "Columns  : " << column << std::endl;
    std::cout << "Row      : " << row    << std::endl;
    std::cout << "Radius   : " << rad << std::endl;
    std::cout << "Vertices : " << packedModel.size() << std::endl;
}

modelSphere::~modelSphere()
{
}

modelTorus::modelTorus(
        std::int_fast32_t row,
        std::int_fast32_t column,
        float irad,
        float orad)
    : row(row), column(column), irad(irad), orad(orad)
{

    packedModel.resize(column * row);
    for(auto i = 0; i <= row; i++){
        vec3 tv, tn;
        vec2 tc;
        float r = M_PI * 2 / row * (float)i;
        float rr = std::cos(r);
        tn.y = std::sin(r);
        for(auto j = 0; j <= column; j++){
            float tr = M_PI * 2 / column * j;
            tv.x = (rr * irad + orad) * std::cos(tr);
            tv.y = tn.y * irad;
            tv.z = (rr * irad + orad) * std::sin(tr);
            tn.x = rr * std::cos(tr);
            tn.z = rr * std::sin(tr);
            // TODO: Add texture coords here
            if (type == vboFormat::INTERLEAVE) {
                (packedModel.at(i * column + j)).vPosition = tv;
                (packedModel.at(i * column + j)).vNormal = tn;
                // packedModel.at(i * column + j).vTexCoord = tc;
            } else if (type == vboFormat::SEPARATE) {
                vertices.push_back(tv);
                normals.push_back(tn);
                // textureCoords.push_back(tc);
                vertColor.push_back(HSLToRGB((float)(i * row + j)/(row * column), 0.5f, 0.5f));
            }

        }
    }

    // Create index
    std::int_fast32_t r = 0;
    faces.resize(row * column * 6);
    for(auto i = 0; i < row; i++){
        for(auto ii = 0; ii < column; ii++){
            r = (column + 1) * i + ii;
            faces.push_back(r             );
            faces.push_back(r + column + 1);
            faces.push_back(r          + 1);
            faces.push_back(r + column + 1);
            faces.push_back(r + column + 2);
            faces.push_back(r          + 1);
        }
    }
}

modelTorus::~modelTorus()
{
}
