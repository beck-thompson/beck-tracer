//
// Created by beck on 8/28/25.
//

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "definitions.h"

int GetIntersection(struct Vector3 ray, struct Vector3 rayOffset, struct World* world, struct Vector3* HitLocation, struct Vector3* Normal, struct Color* Color);

float GetLightIntensity(struct Vector3 point, struct Vector3 normal, struct World* world);

struct Color DrawRay(int x, int y, struct World* world, struct Camera* camera, FILE* fileptr);

void ClearWorld(struct World* world);
struct World* AllocateWorld(void);

void AddSphere(struct World* world, struct Sphere sphere);
void AddTriangle(struct World* world, struct Triangle triangle);

#endif // RAYTRACER_H