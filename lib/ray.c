//
// Created by beck on 8/28/25.
//

#include "ray.h"
#include <math.h>
#include <assert.h>
#include "definitions.h"
#include "../unit_tests/unit_test_helpers.h"

// TODO: move all these helper functions into their own file or something...

// https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
struct Vector3 SnellLaw(struct Vector3 Normal, struct Vector3 i, float n1, float n2)
{
    float u = n1/n2;
    float inside = 1 - (u*u) * (1 - powf(VectDot(Normal, i), 2));
    if (inside <0)
        assert(0);
    struct Vector3 left = VectMult(Normal, sqrtf(inside));
    struct Vector3 right = VectMult((VectSubtract(i, VectMult(Normal, VectDot(Normal,i)))), u);
    return VectAdd(left, right);
}

float QuadraticSolveSml(float a, float b, float c)
{
    float inside = b*b - 4*a*c;
    if (inside < 0)
        return -1;

    float v1 = (-b + sqrtf(inside)) / (2 * a);
    float v2 = (-b - sqrtf(inside)) / (2 * a);

    if (v1 < v2)
        return v1;
    else
        return v2;
}

struct Color ColorMultNoOverflow(struct Color color, float mult)
{
    assert(mult >= 0);

    return (struct Color)
    {
        color.R * mult > 255 ? 255 : color.R * mult,
        color.G * mult > 255 ? 255 : color.G * mult,
        color.B * mult > 255 ? 255 : color.B * mult,
        color.A * mult > 255 ? 255 : color.A * mult,
    };
}

struct Color ColorAddNoOverflow(struct Color a, struct Color b)
{
    return (struct Color)
    {
        a.R + b.R > 255 ? 255 : a.R + b.R,
        a.G + b.G > 255 ? 255 : a.G + b.G,
        a.B + b.B > 255 ? 255 : a.B + b.B,
        a.A + b.A > 255 ? 255 : a.A + b.A,
    };
}

struct Intersection GetFirstIntersection(struct Vector3 incomingRay, struct Vector3 rayOffset, struct World* world)
{
    assert(fabs(VectLength(incomingRay)) -1 <= 0.0001f);

    struct Vector3 ray = VectNormalize(incomingRay);

    int found = 0;
    float smallestRayMultiple = FLT_MAX;

    struct Vector3 hitLocation;
    struct Vector3 normal;
    struct ObjectProperties objectProperties;

    for (int i=0; i<world->NumObjects; i++)
    {
        switch (world->ObjectTypes[i])
        {
            case InvalidObject:
            {
                assert(0); // Invalid object
            }
            case Sphere:
            {
                struct Sphere* sphere = &world->ObjectData[i].Sphere;
                struct Vector3 CO = VectSubtract(rayOffset, sphere->Center);

                float a = VectDot(ray, ray);
                float b = 2 * VectDot(CO, ray);
                float c = VectDot(CO, CO) - (sphere->R * sphere->R);

                // direction vector times this will get you the point on the sphere
                float res = QuadraticSolveSml(a,b,c);
                if (res == -1)
                    continue;

                if (res >= smallestRayMultiple)
                    continue;

                hitLocation = VectAdd(rayOffset, VectMult(ray, res));
                normal = VectNormalize(VectSubtract(hitLocation, sphere->Center));
                objectProperties = sphere->ObjectProperties;
                smallestRayMultiple = res;
                found = 1;

                break;
            }
            case Triangle:
            {
                struct Triangle* triangle = &world->ObjectData[i].Triangle;
                // If this the dot of these is 0, that means the ray will NEVER intersect with the plane
                // (or infinitely intersects)

                // if negative that means the ray is going towards the normal basically.
                float denominator = VectDot(ray, triangle->Normal);
                if (fabs(denominator) < 1e-6f)
                    continue;

                // any of the points would work, they jut need to be on the triangle
                float numerator = VectDot(VectSubtract(triangle->P3, rayOffset), triangle->Normal);
                float solution = numerator / denominator;

                // If it's too far away, don't even bother checking that it intersects the triangle
                if (solution >= smallestRayMultiple)
                    continue;

                // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html

                // TODO: use the more efficient method for this - see bottom of pdf.
                // https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf
                struct Vector3 hitPoint = VectAdd(rayOffset, VectMult(ray, solution));

                struct Vector3 v0 = VectSubtract(triangle->P2, triangle->P1);
                struct Vector3 v1 = VectSubtract(triangle->P3, triangle->P1);
                struct Vector3 v2 = VectSubtract(hitPoint, triangle->P1);

                float d00 = VectDot(v0, v0);
                float d01 = VectDot(v0, v1);
                float d11 = VectDot(v1, v1);
                float d20 = VectDot(v2, v0);
                float d21 = VectDot(v2, v1);
                float denom = d00 * d11 - d01 * d01;
                float v = (d11 * d20 - d01 * d21) / denom;
                float w = (d00 * d21 - d01 * d20) / denom;
                if (v < 0 || w < 0 || (v + w) > 1)
                    continue;

                hitLocation = hitPoint;
                normal = triangle->Normal;
                objectProperties = triangle->ObjectProperties;
                smallestRayMultiple = solution;
                found = 1;

                break;
            }
        }
    }

    if (found == 0)
    {
        return (struct Intersection)
        {
            .Hit = 0,
        };
    }

    return (struct Intersection)
    {
        1,
        hitLocation,
        normal,
        objectProperties,
    };
}


struct IntersectionResults RayCast(struct Vector3 incomingRay, struct Vector3 incomingRayOffset, struct World* world)
{
    assert(fabs(VectLength(incomingRay)) -1 <= 0.0001f);

    struct Vector3 ray = VectNormalize(incomingRay);
    struct Vector3 offset = incomingRayOffset;

    struct IntersectionResults intersectionResults = (struct IntersectionResults)
    {
        .NumIntersections = 0,
    };

    for (int i=0; i<MAX_INTERSECTIONS; i++)
    {
        struct Intersection intersection = GetFirstIntersection(ray, offset, world);

        if (intersection.Hit == 0)
            return intersectionResults;

        intersectionResults.NumIntersections++;
        intersectionResults.Intersections[i] = intersection;

        // TODO: Do a better check than this...
        if (intersection.ObjectProperties.Absorption >= .99)
            return intersectionResults;

        ray = SnellLaw(intersection.Normal, ray, 1.0f, 1.5f); //todo: snells law
        offset = intersection.Location;
    }

    // TODO: Remove this eventually after I feel better about its functionality.
    assert(0); // this should probably never be happening...

    return intersectionResults;
}


float GetLightIntensity(struct Vector3 point, struct Vector3 normal, struct World* world)
{
    // TODO: add assert for ensuring normal is normalized
    normal = VectNormalize(normal);
    float intensity = 0.0f;

    for (int i=0; i<world->NumLightSources; i++)
    {
        switch(world->LightTypes[i])
        {
            case InvalidObject:
            {
                assert(0); // Invalid light
            }
            case PointLight:
            {
                if (VectLength(VectSubtract(point, (struct Vector3){-400, 1000,750})) <= 4.0f)
                {
                    i++;
                    i--;
                }

                struct PointLight* pointLight = &world->LightSources[i].PointLight;

                struct Vector3 LightDirection = VectNormalize(VectSubtract(pointLight->Location, point));
                // light dot normal
                float ldna = VectDot(LightDirection, normal);

                // printf("%f\n", ldna);

                if (fabs(ldna) <= 0.0001)
                    continue;
                // todo: fix this? just do it normally not sus
                ldna = fabs(ldna);

                // You need to face out with the distance squared otherwise its too slow and it gets weird
                intensity += ldna * pointLight->Intensity / (VectLength(VectSubtract(pointLight->Location, point)));// * VectLength(VectSubtract(pointLight->Location, point)));
                break;
            }
            case DirectionalLight:
            {
                struct DirectionalLight* directionalLight = &world->LightSources[i].DirectionalLight;

                // struct Vector3 LightDirection = VectNormalize(directionalLight->Direction);
                // todo: assert directional light is normalized

                // light dot normal
                float ldnb = VectDot(directionalLight->Direction, normal);
                if (ldnb <= 0)
                    continue;

                intensity += ldnb * directionalLight->Intensity;

                break;
            }
            case AmbientLight:
            {
                struct AmbientLight* ambientLight = &world->LightSources[i].AmbientLight;
                intensity += ambientLight->Intensity;
                break;
            }
        }
    }

    return intensity;
}

//x,y point on the camera display
struct Color DrawRay(int x, int y, struct World* world, struct Camera* camera, FILE* fileptr)
{
    struct Vector3 direction = VectSubtract((struct Vector3){x,y,camera->ScreenDistance}, camera->Location);
    direction = VectNormalize(direction);
    struct IntersectionResults results = RayCast(direction, camera->Location, world);

    if (results.NumIntersections == 0)
        return (struct Color){0,0,0, 255};

    // Write some text to the file
    if (fileptr != NULL && x%100==0 && y%100==0)
        fprintf(fileptr, "\\operatorname{vector}((%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f))\n", camera->Location.X, camera->Location.Y, camera->Location.Z, 0.0f, 0.0f, 0.0f);
    // fprintf(fptr, "vector((%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f))\n", HitLocation.X, HitLocation.Y, HitLocation.Z, Normal.X, Normal.Y, Normal.Z);
    // fprintf(fptr, "vector((%f,%f,%f),(%f,%f,%f))", HitLocation.X, HitLocation.Y, HitLocation.Z, Normal.X, Normal.Y, Normal.Z);

    // Close the file
    // fclose(fptr);
    // exit(EXIT_SUCCESS);

    // light intensity at that point
    // TODO: do some ray intersection stuff here to ensure you aren't in shadow or somethin
    float lightIntensity = GetLightIntensity(results.Intersections[results.NumIntersections-1].Location, results.Intersections[results.NumIntersections-1].Normal, world);

    struct Color scaledColor = ColorMultNoOverflow(results.Intersections[results.NumIntersections-1].ObjectProperties.Color, lightIntensity);

    return scaledColor;
}

void ClearWorld(struct World* world)
{
    world->NumObjects = 0;
    world->NumLightSources = 0;
}

struct World* AllocateWorld(void)
{
    struct World* world = malloc(sizeof(struct World));
    ClearWorld(world);
    return world;
}

void AddSphere(struct World* world, struct Sphere sphere)
{
    if (world->NumObjects >= MAX_OBJECTS)
        assert(0); // tried to add more than the maximum amount of objects

    world->ObjectTypes[world->NumObjects] = Sphere;
    world->ObjectData[world->NumObjects].Sphere = sphere;
    world->NumObjects++;
}

void AddTriangle(struct World* world, struct Triangle triangle)
{
    if (world->NumObjects >= MAX_OBJECTS)
        assert(0); // tried to add more than the maximum amount of objects

    world->ObjectTypes[world->NumObjects] = Triangle;
    world->ObjectData[world->NumObjects].Triangle = triangle;
    world->NumObjects++;
}