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

// Matrix functions header file
// Reference : http://d.hatena.ne.jp/white_wheels/20100317/p2

#ifndef Mat4x4_H
#define Mat4x4_H

#define _USE_MATH_DEFINES
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <array>
#include <cmath>

#define deg_to_rad(deg) (((deg)/360)*M_PI)
#define rad_to_deg(rad) (((rad)/M_PI)*360)

class Mat4x4 {
    // Matrix is Column-major order
    // / m00 m01 m02 m03 \
    // | m10 m11 m12 m13 |
    // | m20 m21 m22 m23 |
    // \ m30 m31 m32 m33 /
    // On the memory, each object order is,
    // m00, m10, m20, m30, m01, m11, ... ,m22, m23, m03, m13, m23, m33

    public:
        static const std::int_fast32_t ROW = 4;
        static const std::int_fast32_t COL = 4;
    private:
        std::array<float, ROW*COL> val;
    public:
        // Constructor
        Mat4x4() {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    val[i + j * ROW] = 0.0f;
                }
            }
        }

        Mat4x4(float f) {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    if (i == j)
                        val[i + j * ROW] = f;
                    else
                        val[i + j * ROW] = 0.0f;
                }
            }
        }

        // Destructor
        ~Mat4x4() {}

        // Assignment op
        Mat4x4& operator=(const Mat4x4& A) noexcept {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    this->val[i + j * ROW] = A(i, j);
                }
            }
            return *this;
        }

        // +Mat4x4
        Mat4x4 operator+() noexcept {
            return *this;
        }

        // -Mat4x4
        Mat4x4 operator-() noexcept {
            Mat4x4 A;
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    A(i ,j) = -val[i + j * ROW];
                }
            }
            return A;
        }

        // +=
        Mat4x4& operator+=(const Mat4x4& A) noexcept {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    val[i + j * ROW] += A(i, j);
                }
            }
            return *this;
        }

        // -=
        Mat4x4& operator-=(const Mat4x4& A) noexcept {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    val[i + j * ROW] -= A(i, j);
                }
            }
            return *this;
        }

        // *=
        Mat4x4& operator*=(float k) noexcept {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    val[i + j * ROW] *= k;
                }
            }
            return *this;
        }

        // /=
        Mat4x4& operator/=(float k) noexcept {
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    val[i + j * ROW] /= k;
                }
            }
            return *this;
        }

        // Comparator
        bool operator==(const Mat4x4& A ) const noexcept {
            bool result=true;
            for (auto j = 0; j < COL; j++) {
                for (auto i = 0; i < ROW; i++) {
                    result &= (val[i + j * ROW] == A(i, j));
                }
            }
            return result;
        }

        bool operator!=(const Mat4x4& A ) const noexcept {
            return !((*this) == A);
        }

        // (i)
        float& operator()(std::int_fast32_t i) const noexcept {
            return const_cast<float&>(val[i]);
        }

        // (row, column)
        float& operator()(std::int_fast32_t row,std::int_fast32_t column) const noexcept {
            return const_cast<float&>(val[row + column * ROW]);
        }
};

Mat4x4 operator+(const Mat4x4& A, const Mat4x4& B);
Mat4x4 operator-(const Mat4x4& A, const Mat4x4& B);
Mat4x4 operator*(float k, const  Mat4x4& A);
Mat4x4 operator*(const Mat4x4& A,float k);
Mat4x4 operator/(const Mat4x4& A,float k);
Mat4x4 operator*(const Mat4x4& A, const Mat4x4& B);
Mat4x4 transpose(const Mat4x4& A);
Mat4x4 inverse(const Mat4x4 A);
Mat4x4 lookAtMatrix(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz);
Mat4x4 perspectiveMatrix(float left, float right, float bottom, float top, float near, float far);
Mat4x4 perspectiveMatrix(float fovy, float aspect, float near, float far);
Mat4x4 orthogonalMatrix(float left, float right, float bottom, float top, float near, float far);
Mat4x4 translateMatrix(float x, float y, float z);
Mat4x4 scaleMatrix(float x, float y, float z);
Mat4x4 rotateXMatrix(float r);
Mat4x4 rotateYMatrix(float r);
Mat4x4 rotateZMatrix(float r);
std::ostream& operator<<(std::ostream& s, const Mat4x4& A);

#endif
