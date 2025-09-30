//
// Created by beck on 8/26/25.
//

#include "../lib/vector_math.h"
#include "unit_test_helpers.h"

int VectorDotTests(int* totalTests, int* successfulTests)
{
    int success = 1;
    struct Vector3 v1 = (struct Vector3){0, 0, 0};
    struct Vector3 v2 = (struct Vector3){1, 0, 0};
    struct Vector3 v3 = (struct Vector3){0, 1, 0};
    struct Vector3 v4 = (struct Vector3){0, 0, 1};
    struct Vector3 v5 = (struct Vector3){5, 2, 10};

    // Two perpendicular vectors are 0
    HandleTestResolveSuccess(SimpleFloatEquals(VectDot(v1, v2), 0.0f),
        totalTests, successfulTests, &success);

    // Two perpendicular vectors are 0
    HandleTestResolveSuccess(SimpleFloatEquals(VectDot(v3, v4), 0.0f),
        totalTests, successfulTests, &success);

    // Something dotted with itself is the length squared.
    HandleTestResolveSuccess(SimpleFloatEquals(VectDot(v5, v5), 129.0f),
        totalTests, successfulTests, &success);

    // "random" dot product
    HandleTestResolveSuccess(SimpleFloatEquals(VectDot(v2, v5), 5.0f),
        totalTests, successfulTests, &success);

    // Order doesn't matter
    HandleTestResolveSuccess(SimpleFloatEquals(VectDot(v1, v5), VectDot(v5, v1)),
        totalTests, successfulTests, &success);

    return success;
}

int VectorCrossTests(int* totalTests, int* successfulTests)
{
    int success = 1;
    struct Vector3 v1 = (struct Vector3){0, 0, 0};
    struct Vector3 v2 = (struct Vector3){1, 0, 0};
    struct Vector3 v3 = (struct Vector3){0, 1, 0};
    struct Vector3 v4 = (struct Vector3){0, 0, 1};
    struct Vector3 v5 = (struct Vector3){5, 2, 10};

    // dotting two normalized basis vectors should get you the third one
    HandleTestResolveSuccess(SimpleVectEquals(VectCross(v2, v3), v4),
        totalTests, successfulTests, &success);

    // Switching the order should get you negative
    HandleTestResolveSuccess(SimpleVectEquals(VectCross(v2, v3), VectMult(VectCross(v3, v2), -1.0f)),
        totalTests, successfulTests, &success);

    // "random" cross product
    HandleTestResolveSuccess(SimpleVectEquals(VectCross(v4, v5), (struct Vector3){-2, 5, 0}),
        totalTests, successfulTests, &success);

    // Crossing with yourself is zero
    HandleTestResolveSuccess(SimpleVectEquals(VectCross(v5, v5), v1),
        totalTests,successfulTests, &success);

    return success;
}

// TODO: Extend these, they are very minimal at the moment.
void VectorTests(int* totalTests, int* successfulTests)
{
    HandleTestSectionFinish(VectorDotTests(totalTests, successfulTests), "Vector dot tests");

    HandleTestSectionFinish(VectorCrossTests(totalTests, successfulTests), "Vector cross tests");
}
