//
// Created by beck on 8/28/25.
//

#include <assert.h>

#include "../lib/definitions.h"
#include "colors.def"

// Very simple float equality checker, only works because I'm using pretty "normal" numbers in the tests.
int SimpleFloatEquals(float a, float b)
{
    const float EPSILON = 0.00001f;

    if (fabs(a - b) < EPSILON)
        return 1;

    return 0;
}

int SimpleVectEquals(struct Vector3 v1, struct Vector3 v2)
{
    if (   SimpleFloatEquals(v1.X, v2.X)
        && SimpleFloatEquals(v1.Y, v2.Y)
        && SimpleFloatEquals(v1.Z, v2.Z))
        return 1;

    return 0;
}

void HandleTestResolveSuccess(int res, int* totalTests, int* successfulTests, int* success)
{
    assert(res == 0 || res == 1);

    (*totalTests)++; // dotting two normalized basis vectors should get you the third one
    *successfulTests += res;
    *success &= res;
}

void HandleTestResolveFailure(int res, int* totalTests, int* successfulTests, int* success)
{
    HandleTestResolveSuccess(res == 0, totalTests, successfulTests, success);
}

// If I ever pass in a non compile time test name I will riot...
void HandleTestSectionFinish(int success, char* testName)
{
    if (success)
        printf("  %s tests " ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET "\n", testName);
    else
        printf("  %s tests " ANSI_COLOR_RED "FAILED" ANSI_COLOR_RESET "\n", testName);
}
