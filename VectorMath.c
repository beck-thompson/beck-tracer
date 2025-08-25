#include <math.h>

//
// Created by beck on 8/24/25.
//

struct Vector3
{
    float X;
    float Y;
    float Z;
};

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
