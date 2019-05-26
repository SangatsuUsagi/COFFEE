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

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

struct vec4 {
    float r;
    float g;
    float b;
    float a;
};

// Unified model data structure
struct vec3 {
    float x;
    float y;
    float z;

    // vec3 = vec3
    vec3& operator=(const vec3& v) & {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    };
    // vec3 * scalar
    vec3 operator*(const float& f) & {
        vec3 r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;
        return r;
    };
    // vec3 / scaler
    vec3 operator/(const float& f) & {
        vec3 r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;
        return r;
    };
    // vec3 + scalar
    vec3 operator+(const float& f) & {
        vec3 r;
        r.x = x + f;
        r.y = y + f;
        r.z = z + f;
        return r;
    };
    // vec3 + vec3
    vec3 operator+(const vec3& v) & {
        vec3 r;
        r.x = x + v.x;
        r.y = y + v.y;
        r.z = z + v.z;
        return r;
    };
    // vec3 - scalar
    vec3 operator-(const float& f) & {
        vec3 r;
        r.x = x - f;
        r.y = y - f;
        r.z = z - f;
        return r;
    };
    // vec3 - vec3
    vec3 operator-(const vec3& v) & {
        vec3 r;
        r.x = x - v.x;
        r.y = y - v.y;
        r.z = z - v.z;
        return r;
    };
};

struct vec2 {
    float u;
    float v;

    // vec2 = vec2
    vec2& operator=(const vec2& t) & {
        u = t.u;
        v = t.v;
        return *this;
    };
};

// Packed vertex
struct packedVertex {
    vec3 vPosition;
    vec3 vNormal;
    vec2 vTexCoord;
};

#endif
