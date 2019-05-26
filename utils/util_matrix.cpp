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

// Matrix assessor functions
// Reference : http://d.hatena.ne.jp/white_wheels/20100317/p2

#include <stdexcept>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <array>
#include "util_matrix.hpp"

// (Mat4x4)+(Mat4x4)
Mat4x4 operator+(const Mat4x4& A,const Mat4x4& B) {
    Mat4x4 C;
    for (auto j = 0; j < Mat4x4::COL; j++) {
        for (auto i = 0; i < Mat4x4::ROW; i++) {
            C(i, j) = A(i, j) + B(i, j);
        }
    }
    return C;
}

// (Mat4x4)-(Mat4x4)
Mat4x4 operator-(const Mat4x4& A,const Mat4x4& B) {
    Mat4x4 C;
    for (auto j = 0; j < Mat4x4::COL; j++) {
        for (auto i = 0; i < Mat4x4::ROW; i++) {
            C(i, j) = A(i, j) - B(i, j);
        }
    }
    return C;
}

// float*(Mat4x4)
Mat4x4 operator*(float k,const  Mat4x4& A) {
    Mat4x4 B;
    for (auto j = 0; j < Mat4x4::COL; j++) {
        for (auto i = 0; i < Mat4x4::ROW; i++) {
            B(i, j) = A(i, j) * k;
        }
    }
    return B;
}

// (Mat4x4)*float
Mat4x4 operator*(const Mat4x4& A, float k) {
    Mat4x4 B;
    for (auto j = 0; j < Mat4x4::COL; j++) {
        for (auto i = 0; i < Mat4x4::ROW; i++) {
            B(i, j) = A(i, j) * k;
        }
    }
    return B;
}

// (Mat4x4)/float
Mat4x4 operator/(const Mat4x4& A, float k) {
    Mat4x4 B;
    if (k > FLT_MIN) {
        for (auto j = 0; j < Mat4x4::COL; j++) {
            for (auto i = 0; i < Mat4x4::ROW; i++) {
                B(i, j) = A(i, j) / k;
            }
        }
    } else
        throw std::range_error("Invalid parameter causes div by zero.");
    return B;
}

// A * B
Mat4x4 operator*(const Mat4x4& A,const Mat4x4& B) {
    Mat4x4 C;//C=O

    std::array<float, 4> sum;
    float temp;

    for (auto k = 0; k < 4; k++) {
        sum[0] = B(     k);
        sum[1] = B( 4 + k);
        sum[2] = B( 8 + k);
        sum[3] = B(12 + k);
        for (auto j = 0; j < 4; j++) {
            temp = A(k * 4 + j);
            C(     j) += sum[0] * temp;
            C( 4 + j) += sum[1] * temp;
            C( 8 + j) += sum[2] * temp;
            C(12 + j) += sum[3] * temp;
        }
    }
    return C;
}

// Transpose matrix
Mat4x4 transpose(const Mat4x4& A) {
    Mat4x4 AT;
    for (auto j = 0; j < Mat4x4::COL; j++) {
        for (auto i = 0; i < Mat4x4::ROW; i++) {
            AT(i, j) = A(j, i);
        }
    }
    return AT;
}

// Inverse matrix with using reduced row elimination
// Reference : Matrix Inverse
// https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
Mat4x4 inverse(const Mat4x4 A) {
    Mat4x4 AI;
    for (auto column = 0; column < Mat4x4::COL; ++column) {
        AI(column, column) = 1.0f;
    }
    for (auto column = 0; column < Mat4x4::COL; ++column) {
        // Swap row in case our pivot point is not working
        if (std::abs(A(column, column)) < FLT_EPSILON) {
            auto big = column;
            for (auto row = column; row < Mat4x4::ROW; ++row)
                if (std::abs(A(row, column)) > std::abs(A(big, column))) big = row;
            // Print this is a singular matrix, return identity ?
            if (big == column) {
                throw std::range_error("Singular matrix");
            }
            // Swap rows
            else for (auto col = 0; col < Mat4x4::ROW; ++col) {
                std::swap(A(column, col), A(big, col));
                std::swap(AI(column, col), AI(big, col));
            }
        }
        // Set each row in the column to 0
        for (auto row = 0; row < Mat4x4::ROW; ++row) {
            if (row != column) {
                float coeff = A(row, column) / A(column, column);
                if (coeff != 0) {
                    for (auto j = 0; j < Mat4x4::COL ; ++j) {
                        A(row, j) -= coeff * A(column, j);
                        AI(row, j) -= coeff * AI(column, j);
                    }
                    // Set the element to 0 for safety
                    A(row, column) = 0;
                }
            }
        }
    }
    // Set each element of the diagonal to 1
    for (auto row = 0; row < Mat4x4::ROW; ++row) {
        for (auto column = 0; column < Mat4x4::COL; ++column) {
            AI(row, column) /= A(row, row);
        }
    }

    return AI;
}

// Viewing transformation
Mat4x4 lookAtMatrix(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz)
{
    Mat4x4 mLookAt;
    float l;

    tx = ex - tx;
    ty = ey - ty;
    tz = ez - tz;
    l = sqrt(tx * tx + ty * ty + tz * tz);
    if (l < FLT_MIN) {
        throw std::range_error("Invalid parameter causes div by zero.");
    }
    mLookAt( 2) = tx / l;
    mLookAt( 6) = ty / l;
    mLookAt(10) = tz / l;

    tx = uy * mLookAt(10) - uz * mLookAt( 6);
    ty = uz * mLookAt( 2) - ux * mLookAt(10);
    tz = ux * mLookAt( 6) - uy * mLookAt( 2);
    l = sqrt(tx * tx + ty * ty + tz * tz);
    if (l < FLT_MIN) {
        throw std::range_error("Invalid parameter causes div by zero.");
    }

    mLookAt( 0) = tx / l;
    mLookAt( 4) = ty / l;
    mLookAt( 8) = tz / l;

    mLookAt( 1) = mLookAt( 6) * mLookAt( 8) - mLookAt(10) * mLookAt( 4);
    mLookAt( 5) = mLookAt(10) * mLookAt( 0) - mLookAt( 2) * mLookAt( 8);
    mLookAt( 9) = mLookAt( 2) * mLookAt( 4) - mLookAt( 6) * mLookAt( 0);

    mLookAt(12) = -(ex * mLookAt( 0) + ey * mLookAt( 4) + ez * mLookAt( 8));
    mLookAt(13) = -(ex * mLookAt( 1) + ey * mLookAt( 5) + ez * mLookAt( 9));
    mLookAt(14) = -(ex * mLookAt( 2) + ey * mLookAt( 6) + ez * mLookAt(10));

    mLookAt( 3) = 0.0f;
    mLookAt( 7) = 0.0f;
    mLookAt(11) = 0.0f;

    mLookAt(15) = 1.0f;

    return mLookAt;
}

// Perspective matrix
Mat4x4 perspectiveMatrix(float left, float right, float bottom, float top, float near, float far)
{
    Mat4x4 mFrus;
    float dx = right - left;
    float dy = top - bottom;
    float dz = far - near;

    if ((dx < FLT_MIN) || (dy < FLT_MIN) || (dz < FLT_MIN)) {
        throw std::range_error("Invalid parameter causes div by zero.");
    }

    mFrus( 0) =  2.0f * near / dx;
    mFrus( 1) =  0.0f;
    mFrus( 2) =  0.0f;
    mFrus( 3) =  0.0f;
    mFrus( 4) =  0.0f;
    mFrus( 5) =  2 * near / dy;
    mFrus( 6) =  0.0f;
    mFrus( 7) =  0.0f;
    mFrus( 8) =  (right + left) / dx;
    mFrus( 9) =  (top + bottom) / dy;
    mFrus(10) =  - (far + near) / dz;
    mFrus(11) = -1.0f;
    mFrus(12) =  0.0f;
    mFrus(13) =  0.0f;
    mFrus(14) =  -2.0f * far * near / dz;
    mFrus(15) =  0.0f;

    return mFrus;
}

// Perspective matrix
Mat4x4 perspectiveMatrix(float fovy, float aspect, float near, float far)
{
    float ymax = near * std::tan(fovy);
    float ymin = -ymax;
    float xmin = ymin * aspect;
    float xmax = ymax * aspect;

    return perspectiveMatrix(xmin, xmax, ymin, ymax, near, far);
}

// Orthogonal matrix
Mat4x4 orthogonalMatrix(float left, float right, float bottom, float top, float near, float far)
{
    Mat4x4 mOrtho;
    float dx = right - left;
    float dy = top - bottom;
    float dz = far - near;

    if ((dx < FLT_MIN) || (dy < FLT_MIN) || (dz < FLT_MIN)) {
        throw std::range_error("Invalid parameter causes div by zero.");
    }

    mOrtho( 0) =  2.0f / dx;
    mOrtho( 1) =  0.0f;
    mOrtho( 2) =  0.0f;
    mOrtho( 3) =  0.0f;
    mOrtho( 4) =  0.0f;
    mOrtho( 5) =  2.0f / dy;
    mOrtho( 6) =  0.0f;
    mOrtho( 7) =  0.0f;
    mOrtho( 8) =  0.0f;
    mOrtho( 9) =  0.0f;
    mOrtho(10) = -2.0f / dz;
    mOrtho(11) =  0.0f;
    mOrtho(12) = -(right + left) / dx;
    mOrtho(13) = -(top + bottom) / dy;
    mOrtho(14) = -(far + near) / dz;
    mOrtho(15) =  1.0f;

    return mOrtho;
}

// Translate
Mat4x4 translateMatrix(float x, float y, float z)
{
    Mat4x4 mTrans;

    mTrans( 0) = 1.0f;
    mTrans( 5) = 1.0f;
    mTrans(10) = 1.0f;
    mTrans(15) = 1.0f;

    mTrans(12) = x;
    mTrans(13) = y;
    mTrans(14) = z;

    return mTrans;
}

// Scale
Mat4x4 scaleMatrix(float x, float y, float z)
{
    Mat4x4 mScale;

    mScale( 0) = x;
    mScale( 5) = y;
    mScale(10) = z;
    mScale(15) = 1.0f;

    return mScale;
}

// Rotate X, Y, Z
Mat4x4 rotateXMatrix(float r)
{
    Mat4x4 mRotate;

    mRotate( 0) = 1.0f;
    mRotate( 5) =  std::cos(r);
    mRotate( 6) =  std::sin(r);
    mRotate( 9) = -std::sin(r);
    mRotate(10) =  std::cos(r);
    mRotate(15) = 1.0f;

    return mRotate;
}

Mat4x4 rotateYMatrix(float r)
{
    Mat4x4 mRotate;

    mRotate( 0) =  std::cos(r);
    mRotate( 2) = -std::sin(r);
    mRotate( 5) = 1.0f;
    mRotate( 8) =  std::sin(r);
    mRotate(10) =  std::cos(r);
    mRotate(15) = 1.0f;

    return mRotate;
}

Mat4x4 rotateZMatrix(float r)
{
    Mat4x4 mRotate;

    mRotate( 0) =  std::cos(r);
    mRotate( 1) =  std::sin(r);
    mRotate( 4) = -std::sin(r);
    mRotate( 5) =  std::cos(r);
    mRotate(10) = 1.0f;
    mRotate(15) = 1.0f;

    return mRotate;
}

// Output matrix for debug
//mat4x4((0.013258, 0.213597, -0.196397, 0.109180), (-0.103030, -0.343319, 0.264110, -0.348891), (0.012466, 0.224092, -0.208208, 0.163975), (0.204618, -0.019554, -0.184286, -0.007047))
std::ostream& operator<<(std::ostream& s, const Mat4x4& A) {

    s << std::fixed;
    s << "mat4x4(";

    for (auto i = 0; i < Mat4x4::ROW; i++) {
        s << "(";
        for (auto j = 0; j < Mat4x4::COL; j++) {
            s << A(i, j);
            if (j != Mat4x4::COL -1) s <<", ";
        }
        if (i != Mat4x4::ROW -1)
            s <<"), ";
        else
            s << ")";
    }
    s << ")";

    s << std::defaultfloat;

    return s;
}
