#include <math.h>
#include "vector_math.h"

//
// Created by beck on 8/24/25.
//

struct Vector3 VectSubtract(struct Vector3 v1, struct Vector3 v2)
{
    return (struct Vector3){ v1.X-v2.X, v1.Y-v2.Y, v1.Z-v2.Z };
}

struct Vector3 VectAdd(struct Vector3 v1, struct Vector3 v2)
{
    return (struct Vector3){ v1.X+v2.X, v1.Y+v2.Y, v1.Z+v2.Z };
}

float VectDot(struct Vector3 v1, struct Vector3 v2)
{
    return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
}

struct Vector3 VectCross(struct Vector3 v1, struct Vector3 v2)
{
    return (struct Vector3)
    {
        v1.Y*v2.Z - v1.Z*v2.Y,
        v1.Z*v2.X - v1.X*v2.Z,
        v1.X*v2.Y - v1.Y*v2.X
    };
}

struct Vector3 VectMult(struct Vector3 v, float mult)
{
    return (struct Vector3){ v.X*mult, v.Y*mult, v.Z*mult };
}

float VectLength(struct Vector3 v)
{
    return sqrtf((v.X*v.X) + (v.Y*v.Y) + (v.Z*v.Z));
}

struct Vector3 VectNormalize(struct Vector3 v)
{
    float length = VectLength(v);
    return (struct Vector3){ v.X/length, v.Y/length, v.Z/length };
}

struct Vector3 MatrixVectMult(struct Matrix3 matrix, struct Vector3 vector)
{
    float x = vector.X * matrix.X1 + vector.Y * matrix.X2 + vector.Z * matrix.X3;
    float y = vector.X * matrix.Y1 + vector.Y * matrix.Y2 + vector.Z * matrix.Y3;
    float z = vector.X * matrix.Z1 + vector.Y * matrix.Z2 + vector.Z * matrix.Z3;

    return (struct Vector3){ x, y, z };
}

struct Matrix3 MatrixMatrixMult(struct Matrix3 m1, struct Matrix3 m2)
{
    struct Vector3 v1 = MatrixVectMult(m1, (struct Vector3){ m2.X1, m2.Y1, m2.Z1});
    struct Vector3 v2 = MatrixVectMult(m1, (struct Vector3){ m2.X2, m2.Y2, m2.Z2});
    struct Vector3 v3 = MatrixVectMult(m1, (struct Vector3){ m2.X3, m2.Y3, m2.Z3});

    return (struct Matrix3)
    {
        v1.X, v2.X, v3.X,
        v1.Y, v2.Y, v3.Y,
        v1.Z, v2.Z, v3.Z,
    };
}

float Det(struct Matrix3 m)
{
    return   m.X1 * (m.Y2 * m.Z3 - m.Y3 * m.Z2)
           - m.Y1 * (m.X1 * m.Z3 - m.X3 * m.Z2)
           + m.Z1 * (m.X2 * m.Y3 - m.X3 * m.Y2);

}

struct Matrix3 CreateLinearTransformMatrix(struct Vector3 v)
{
    return (struct Matrix3)
    {
        v.X, 0, 0,
        0, v.Y, 0,
        0, 0, v.Z,
    };
}

struct Matrix3 CreateXRotationMatrix(float angleX)
{
    return (struct Matrix3)
    {
        1, 0, 0,
        0, cosf(angleX), -sinf(angleX),
        0, sinf(angleX), cosf(angleX),
    };
}

struct Matrix3 CreateYRotationMatrix(float angleY)
{
    return (struct Matrix3)
    {
        cosf(angleY), 0, sinf(angleY),
        0, 1, 0,
        -sinf(angleY), 0, cosf(angleY),
    };
}

struct Matrix3 CreateZRotationMatrix(float angleZ)
{
    return (struct Matrix3)
    {
        cosf(angleZ), -sinf(angleZ), 0,
        sinf(angleZ), cosf(angleZ), 0,
        0, 0, 1,
    };
}

struct Matrix3 CreateRotationMatrix(float angleX, float angleY, float angleZ)
{
    struct Matrix3 mX = CreateXRotationMatrix(angleX);
    struct Matrix3 mY = CreateYRotationMatrix(angleY);
    struct Matrix3 mZ = CreateZRotationMatrix(angleZ);

    struct Matrix3 m1 = MatrixMatrixMult(mX, mY);
    struct Matrix3 m2 = MatrixMatrixMult(mZ, m1);

    return m2;
}