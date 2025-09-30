//
// Created by beck on 8/26/25.
//

#include <assert.h>

#include "../lib/definitions.h"
#include "../lib/ray.h"
#include "unit_test_helpers.h"

#define DEFAULT_COLOR (struct Color){100, 100, 100, 255}
#define RANDOM_TEST_AMOUNT 1000

// TODO: Fix this sussyness
void GetThreeRandomIndex(int* one, int* two, int* three)
{
    *one = rand() % 3;

    int potTwo = rand() % 3;
    while (potTwo == *one)
        potTwo = rand() % 3;
    *two = potTwo;

    int potThree= rand() % 3;
    while (potThree == *one || potThree == *two)
        potThree = rand() % 3;
    *three = potThree;
}

int RandomizedPlaneTest(struct Triangle triangle, struct Vector3 IntersectPoint, int shouldHit)
{
    struct Vector3 HitLocation;
    struct Vector3 NormalOut;
    struct Color Color;

    struct Vector3 Points[3];
    Points[0] = triangle.P1;
    Points[1] = triangle.P2;
    Points[2] = triangle.P3;

    struct World* world = AllocateWorld();

    for (int j=0; j<RANDOM_TEST_AMOUNT; j++)
    {
        struct Vector3 randOffset = (struct Vector3)
        {
            (float)(double)rand()/(double)(RAND_MAX/200),
            (float)(double)rand()/(double)(RAND_MAX/200),
            (float)(double)rand()/(double)(RAND_MAX/200),
        };

        // point is already on plane - ignore
        // I also think this technically is impossible to happen but whatever we test it anyway
        if (fabs(VectDot(randOffset, triangle.Normal)) <= 0.0001)
        {
            j--; // skip
            continue;
        }

        struct Vector3 randNormal;
        // I don't think this is perfectly 50 50 but it doesn't matter much so whatever
        if (rand() % 2)
            randNormal = triangle.Normal;
        else
            randNormal = VectMult(triangle.Normal, -1);

        int one, two, three;
        GetThreeRandomIndex(&one, &two, &three);

        // TODO: Ensure normal gets + and -
        AddTriangle(world, (struct Triangle)
        {
            randNormal,
            Points[one], Points[two], Points[three],
            DEFAULT_COLOR,
        });

        struct Vector3 ray = VectSubtract(IntersectPoint, randOffset);

        int result = GetIntersection(ray, randOffset, world, &HitLocation, &NormalOut, &Color);

        if (shouldHit != result)
            return 0;

        ClearWorld(world);

    }

    return 1;
}

// TODO: Add more edge cases (more as well):
//  1. inside sphere
//  2. hitting edge of sphere
int RaySphereIntersectTests(int* totalTests, int* successfulTests) {
    int success = 1;
    struct World* world = AllocateWorld();
    struct Vector3 HitLocation;
    struct Vector3 Normal;
    struct Color Color;

    struct Sphere basicSphere = (struct Sphere)
    {
        10,
        (struct Vector3){0, 0, 100},
        DEFAULT_COLOR,
    };

    AddSphere(world, basicSphere);

    // vector directly facing the sphere hitting it
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 0, 0, 1 }, (struct Vector3){ 0, 0, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);

    // vector missing the sphere (crazy direction)
    HandleTestResolveFailure(GetIntersection((struct Vector3){ 0, 100, 1 }, (struct Vector3){ 0, 0, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);

    // vector missing the sphere (big offset)
    HandleTestResolveFailure(GetIntersection((struct Vector3){ 0, 0, 1 }, (struct Vector3){ 100, 0, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);

    // vector hitting the sphere (odd offset and direction)
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ -100, -100, 100 }, (struct Vector3){ 100, 100, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);

    free(world);
    return success;
}

int RayPlaneIntersectTests(int* totalTests, int* successfulTests) {
    int success = 1;
    struct World* world = AllocateWorld();
    struct Vector3 HitLocation;
    struct Vector3 Normal;
    struct Color Color;

    struct Triangle XY = (struct Triangle)
    {
        (struct Vector3){ 0, 0, -1 },
        (struct Vector3){ 0, 0, 0 },
        (struct Vector3){ 100, 0, 0 },
        (struct Vector3){ 100, 100, 0 },
        DEFAULT_COLOR,
    };

    struct Triangle XZ = (struct Triangle)
    {
        (struct Vector3){ 0, -1, 0 },
        (struct Vector3){ 0, 0, 0 },
        (struct Vector3){ 100, 0, 0 },
        (struct Vector3){ 100, 0, 100 },
        DEFAULT_COLOR,
    };

    struct Triangle ZY = (struct Triangle)
    {
        (struct Vector3){ -1, 0, 0 },
        (struct Vector3){ 0, 0, 0 },
        (struct Vector3){ 0, 100, 0 },
        (struct Vector3){ 0, 100, 100 },
        DEFAULT_COLOR,
    };

    // Ray going into the XY plane directly
    AddTriangle(world, XY);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 0, 0, 1 }, (struct Vector3){ 0, 0, -10 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Ray going into the XZ plane directly
    AddTriangle(world, XZ);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 0, 1, 0 }, (struct Vector3){ 0, -10, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Ray going into the ZY plane the other way
    AddTriangle(world, ZY);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 1, 0, 0 }, (struct Vector3){ -10, 0, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Ray going into the XY plane the other way
    AddTriangle(world, XY);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 0, 0, -1 }, (struct Vector3){ 0, 0, 10 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Ray going into the XZ plane the other way
    AddTriangle(world, XZ);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ 0, -1, 0 }, (struct Vector3){ 0, 10, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Ray going into the ZY plane the other way
    AddTriangle(world, ZY);
    HandleTestResolveSuccess(GetIntersection((struct Vector3){ -1, 0, 0 }, (struct Vector3){ 10, 0, 0 }, world, &HitLocation, &Normal, &Color),
        totalTests, successfulTests, &success);
    ClearWorld(world);

    // Random points hitting normal plane
    HandleTestResolveSuccess(RandomizedPlaneTest(XY, (struct Vector3){10, 5, 0}, 1),
        totalTests, successfulTests, &success);

    // Random testing missing a normal plane
    HandleTestResolveSuccess(RandomizedPlaneTest(XY, (struct Vector3){10, 50, 0}, 0),
        totalTests, successfulTests, &success);

    return success;
}

void RayIntersectTests(int* totalTests, int* successfulTests)
{
    HandleTestSectionFinish(RaySphereIntersectTests(totalTests, successfulTests), "Sphere intersection tests");

    HandleTestSectionFinish(RayPlaneIntersectTests(totalTests, successfulTests), "Plane intersection tests");
}