//
// Created by beck on 8/28/25.
//

#ifndef RAYTRACER_UNIT_TEST_HELPERS_H
#define RAYTRACER_UNIT_TEST_HELPERS_H

int SimpleFloatEquals(float a, float b);

int SimpleVectEquals(struct Vector3 v1, struct Vector3 v2);

void HandleTestResolveSuccess(int res, int* totalTests, int* successfulTests, int* success);
void HandleTestResolveFailure(int res, int* totalTests, int* successfulTests, int* success);

void HandleTestSectionFinish(int success, char* testName);

#endif //RAYTRACER_UNIT_TEST_HELPERS_H