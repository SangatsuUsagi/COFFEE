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

#ifndef UTIL_XLOADER_H_
#define UTIL_XLOADER_H_

#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "util_modelbase.hpp"

class xLoader : public baseModel {

    private:
        // State machine states
        enum X_STATE {
            X_START = 0,
            X_COMMON_HDR,
            X_MESH_HDR,
            X_VERTICE_CNT,
            X_VERTICE_DAT,
            X_FACE_CNT,
            X_FACE_DAT,
            X_MATERIAL_HDR,
            X_MATERIAL_NUM,
            X_MATERIAL_CNT,
            X_MATERIAL_DAT,
            X_MATERIAL0_HDR,
            X_MATERIAL0_DAT,
            X_MATERIAL0_TEX_HDR,
            X_MATERIAL0_TEX_NAM,
            X_NORMAL_HDR,
            X_NORMAL_V_CNT,
            X_NORMAL_V_DAT,
            X_NORMAL_F_CNT,
            X_NORMAL_F_DAT,
            X_UV_HDR,
            X_UV_CNT,
            X_UV_DAT,
            X_END
        };

        // State machine current state and results
        std::int_fast32_t state;

        // generic loop counter
        std::int_fast32_t count;

        // Material
        std::int_fast32_t nFaceIndexes;

        float power;
        std::array<float, 4> faceColor;
        std::array<float, 3> specularColor;
        std::array<float, 3> emissiveColor;

    public:
        // Constructor
        xLoader() {};

        // Destructor
        ~xLoader() {}

        // Model Loader
        void loadModel(const char* fn);
};
#endif
