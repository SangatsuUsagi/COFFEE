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

#ifndef UTIL_MODELGEN_H
#define UTIL_MODELGEN_H

#include <cstdint>
#include <vector>

#include "util_modelbase.hpp"

class modelSphere : public baseModel {

    private:
        const std::int_fast32_t row;
        const std::int_fast32_t column;
        const float rad;
		std::vector<vec4> vertColor;

    public:
        modelSphere(vboFormat type, std::int_fast32_t row, std::int_fast32_t column, float rad);
        ~modelSphere();

        // Return separate data
        vec4*              getVertColor()                     { return &vertColor[0]; }
        vec4*              getVertColor(std::int_fast32_t n)  { return &vertColor[n]; }
        std::int_fast32_t  getVertColorSize()                 { return vertColor.size(); }

		void loadModel(const char* fn) {};
};

class modelTorus : public baseModel {
    private:
        const std::int_fast32_t row;
        const std::int_fast32_t column;
        const float irad;
        const float orad;
		std::vector<vec4> vertColor;

    public:
        modelTorus(std::int_fast32_t row, std::int_fast32_t column, float irad, float orad);
        ~modelTorus();

        // Return separate data
        vec4*              getVertColor()                     { return &vertColor[0]; }
        vec4*              getVertColor(std::int_fast32_t n)  { return &vertColor[n]; }
        std::int_fast32_t  getVertColorSize()                 { return vertColor.size(); }

        void loadModel(const char* fn) {};
};

#endif
