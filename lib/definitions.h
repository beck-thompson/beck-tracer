//
// Created by beck on 8/28/25.
//

#ifndef RAYTRACER_DEFINITIONS_H
#define RAYTRACER_DEFINITIONS_H

#include <SDL2/SDL.h>
#include "vector_math.h"

#define MAX_OBJECTS 100
#define MAX_LIGHTS 100
#define MAX_INTERSECTIONS 5

struct Color
{
    Uint8 R;
    Uint8 G;
    Uint8 B;
    Uint8 A;
};

// TODO: Add color to this
struct ObjectProperties
{
    // TODO: Add more for mirrors etc..

    // How much light is absorbed by the object.
    float Absorption;

    // Speed of light in a vacuum / speed of light in the object
    // Slower speed of light in the object -> bigger refraction index
    // Faster speed -> smaller index
    float RefractionIndex;

    struct Color Color;
};

struct Intersection
{
    int Hit;
    struct Vector3 Location;
    struct Vector3 Normal;
    struct ObjectProperties ObjectProperties;
};

struct IntersectionResults
{
    int NumIntersections;

    struct Intersection Intersections[MAX_INTERSECTIONS];
};

struct Sphere
{
    // Radius of the sphere
    float R;

    // Location of the center of the sphere
    struct Vector3 Center;

    struct ObjectProperties ObjectProperties;
};

struct Triangle
{
    struct Vector3 Normal;

    struct Vector3 P1;
    struct Vector3 P2;
    struct Vector3 P3;

    struct ObjectProperties ObjectProperties;
};

enum ObjectType {
    InvalidObject,
    Sphere,
    Triangle,
};

union ObjectData
{
    struct Sphere Sphere;
    struct Triangle Triangle;
};

struct PointLight
{
    struct Vector3 Location;
    float Intensity;
};

struct DirectionalLight
{
    struct Vector3 Direction;
    float Intensity;
};

struct AmbientLight
{
    float Intensity;
};

enum LightType {
    InvalidLight,
    PointLight,
    DirectionalLight,
    AmbientLight,
};

union LightSource
{
    struct PointLight PointLight;
    struct DirectionalLight DirectionalLight;
    struct AmbientLight AmbientLight;
};

struct World
{
    enum ObjectType ObjectTypes[MAX_OBJECTS];
    union ObjectData ObjectData[MAX_OBJECTS];
    int NumObjects;

    enum LightType LightTypes[MAX_LIGHTS];
    union LightSource LightSources[MAX_LIGHTS];
    int NumLightSources;
};

struct Camera
{
    // Actual location of the camera where the rays start
    struct Vector3 Location;

    // Distance of the screen from the camera location
    float ScreenDistance;
};

#endif //RAYTRACER_DEFINITIONS_H