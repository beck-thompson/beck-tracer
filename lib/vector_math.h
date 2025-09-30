#ifndef VECTOR3MATH_H
#define VECTOR3MATH_H

//
// Created by beck on 8/24/25.
//

struct Vector3
{
    float X;
    float Y;
    float Z;
};

struct Matrix3 {
    float X1; float X2; float X3;
    float Y1; float Y2; float Y3;
    float Z1; float Z2; float Z3;
};

// Vector ops
struct Vector3 VectSubtract(struct Vector3 v1, struct Vector3 v2);
struct Vector3 VectAdd(struct Vector3 v1, struct Vector3 v2);
float VectDot(struct Vector3 v1, struct Vector3 v2);
struct Vector3 VectCross(struct Vector3 v1, struct Vector3 v2);
struct Vector3 VectMult(struct Vector3 v, float mult);
float VectLength(struct Vector3 v);
struct Vector3 VectNormalize(struct Vector3 v);

// Matrix operations
struct Vector3 MatrixVectMult(struct Matrix3 matrix, struct Vector3 vector);
struct Matrix3 MatrixMatrixMult(struct Matrix3 m1, struct Matrix3 m2);
float Det(struct Matrix3 m);

// Matrix creation
struct Matrix3 CreateLinearTransformMatrix(struct Vector3 v);
struct Matrix3 CreateXRotationMatrix(float angleX);
struct Matrix3 CreateYRotationMatrix(float angleY);
struct Matrix3 CreateZRotationMatrix(float angleZ);
struct Matrix3 CreateRotationMatrix(float angleX, float angleY, float angleZ);

#endif // VECTOR3MATH_H