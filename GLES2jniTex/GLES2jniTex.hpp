//
// Copyright (c) 2019 Tatsuya Kobayashi
// All rights reserved.
//

// Based on gles3jni.h from
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

#define MAX_INSTANCES_PER_SIDE  8
#define MAX_INSTANCES           (MAX_INSTANCES_PER_SIDE * MAX_INSTANCES_PER_SIDE)
#define TWO_PI                  (2.0 * M_PI)
#define MAX_ROT_SPEED           (0.3 * TWO_PI)

#define MAX_TEXTURES             4

struct packedVertex {
    GLfloat pos[2];
    GLubyte rgba[4];
    GLfloat tex[2];
};
