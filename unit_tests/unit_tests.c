//
// Created by beck on 8/26/25.
//

#include <stdio.h>
#include <stdlib.h>
#include "vector_tests.c"
#include "ray_intersect_tests.c"

int main(int argc, char **argv) {
    int testToRun = 0;

    if (argc > 2)
    {
        printf("Too many arguments\n");
        return 1;
    }

    if (argc == 1)
    {
        testToRun = -1; // run all tests
    }
    else
    {
        int argument = atoi(argv[1]);
        if (argument <= 0)
        {
            printf("Invalid argument\n");
            return 1;
        }
        testToRun = argument;
    }

    if (testToRun >= 1 || testToRun == -1)
    {
        printf("--- Vector unit tests ---\n");
        int totalTests = 0;
        int successfulTests = 0;

        VectorTests(&totalTests, &successfulTests);

        printf("--- (%i/%i) ---\n", successfulTests, totalTests);
    }

    if (testToRun >= 2 || testToRun == -1)
    {
        printf("--- Ray intersect unit tests ---\n");
        int totalTests = 0;
        int successfulTests = 0;

        RayIntersectTests(&totalTests, &successfulTests);

        printf("--- (%i/%i) ---\n", successfulTests, totalTests);
    }
}
