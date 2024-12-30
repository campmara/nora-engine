#ifndef NORA_VECTOR3_H
#define NORA_VECTOR3_H

#include "nora_defines.h"

namespace nora
{
    struct Vector3
    {
        float32 x;
        float32 y;
        float32 z;

        Vector3 operator*(const Vector3 &v2)
        {
            return { this->x * v2.x, this->y * v2.y, this->z * v2.z };
        }

        Vector3 operator*(const float32 &value)
        {
            return { this->x * value, this->y * value, this->z * value };
        }
    };
}

#endif