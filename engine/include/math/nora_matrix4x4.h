#ifndef NORA_MATRIX4X4_H
#define NORA_MATRIX4X4_H

#include "nora_defines.h"

namespace nora
{
    struct Matrix4x4
    {
        float32 m[4][4];

        Matrix4x4 operator*(const Matrix4x4 &m2)
        {
            return {
                this->m[0][0] * m2.m[0][0] + this->m[0][1] * m2.m[1][0] +
                    this->m[0][2] * m2.m[2][0] + this->m[0][3] * m2.m[3][0],
                this->m[0][0] * m2.m[0][1] + this->m[0][1] * m2.m[1][1] +
                    this->m[0][2] * m2.m[2][1] + this->m[0][3] * m2.m[3][1],
                this->m[0][0] * m2.m[0][2] + this->m[0][1] * m2.m[1][2] +
                    this->m[0][2] * m2.m[2][2] + this->m[0][3] * m2.m[3][2],
                this->m[0][0] * m2.m[0][3] + this->m[0][1] * m2.m[1][3] +
                    this->m[0][2] * m2.m[2][3] + this->m[0][3] * m2.m[3][3],
                this->m[1][0] * m2.m[0][0] + this->m[1][1] * m2.m[1][0] +
                    this->m[1][2] * m2.m[2][0] + this->m[1][3] * m2.m[3][0],
                this->m[1][0] * m2.m[0][1] + this->m[1][1] * m2.m[1][1] +
                    this->m[1][2] * m2.m[2][1] + this->m[1][3] * m2.m[3][1],
                this->m[1][0] * m2.m[0][2] + this->m[1][1] * m2.m[1][2] +
                    this->m[1][2] * m2.m[2][2] + this->m[1][3] * m2.m[3][2],
                this->m[1][0] * m2.m[0][3] + this->m[1][1] * m2.m[1][3] +
                    this->m[1][2] * m2.m[2][3] + this->m[1][3] * m2.m[3][3],
                this->m[2][0] * m2.m[0][0] + this->m[2][1] * m2.m[1][0] +
                    this->m[2][2] * m2.m[2][0] + this->m[2][3] * m2.m[3][0],
                this->m[2][0] * m2.m[0][1] + this->m[2][1] * m2.m[1][1] +
                    this->m[2][2] * m2.m[2][1] + this->m[2][3] * m2.m[3][1],
                this->m[2][0] * m2.m[0][2] + this->m[2][1] * m2.m[1][2] +
                    this->m[2][2] * m2.m[2][2] + this->m[2][3] * m2.m[3][2],
                this->m[2][0] * m2.m[0][3] + this->m[2][1] * m2.m[1][3] +
                    this->m[2][2] * m2.m[2][3] + this->m[2][3] * m2.m[3][3],
                this->m[3][0] * m2.m[0][0] + this->m[3][1] * m2.m[1][0] +
                    this->m[3][2] * m2.m[2][0] + this->m[3][3] * m2.m[3][0],
                this->m[3][0] * m2.m[0][1] + this->m[3][1] * m2.m[1][1] +
                    this->m[3][2] * m2.m[2][1] + this->m[3][3] * m2.m[3][1],
                this->m[3][0] * m2.m[0][2] + this->m[3][1] * m2.m[1][2] +
                    this->m[3][2] * m2.m[2][2] + this->m[3][3] * m2.m[3][2],
                this->m[3][0] * m2.m[0][3] + this->m[3][1] * m2.m[1][3] +
                    this->m[3][2] * m2.m[2][3] + this->m[3][3] * m2.m[3][3],
            };
        }
    };
}

#endif