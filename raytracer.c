#include <float.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "lib/vector_math.h"
#include "lib/definitions.h"
#include "lib/ray.h"
#include <time.h>
#include <stdio.h>

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1.0e6;
}

#define RED (struct Color){ 240, 20, 20, 255 }
#define GREEN (struct Color){ 20, 240, 20, 255 }
#define BLUE (struct Color){ 20, 20, 240, 255 }

void CreateObjects(struct World* world)
{
    AddSphere(world, (struct Sphere)
    {
        100,
        (struct Vector3){0, 0,200},
        (struct ObjectProperties) {0.8f, 0.5f, RED},
    });

    AddSphere(world, (struct Sphere)
    {
        100,
        (struct Vector3){50, 0,300},
        (struct ObjectProperties) {1.0f, 1.0f, GREEN},
    });
}

void CreateLights(struct World* world)
{
    world->NumLightSources = 1;
    world->LightTypes[0] = PointLight;

    world->LightSources[0].PointLight = (struct PointLight){(struct Vector3){300, 0,700}, 1200.0f};
    // printf("Mouse Position: %d, %d\n", xPos, yPos);

    world->NumLightSources = 2;
    world->LightTypes[1] = AmbientLight;

    world->LightSources[1].AmbientLight = (struct AmbientLight){0.0f};


    world->NumLightSources = 3;
    world->LightTypes[2] = DirectionalLight;

    world->LightSources[2].DirectionalLight = (struct DirectionalLight)
    {
        (struct Vector3){-1,2,5},
        0.0f,
    };
}

struct World* CreateWorld(void)
{
    struct World* world = AllocateWorld();

    CreateObjects(world);
    CreateLights(world);

    return world;
}

struct Camera* CreateCamera(void)
{
    struct Camera* camera = malloc(sizeof(struct Camera));
    camera->Location = (struct Vector3){ 0,0,-1160 };
    camera->ScreenDistance = -500;
    return camera;
}

int main(void)
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 1000, 0, &window, &renderer);
    SDL_RenderSetScale(renderer,1,1);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen("out.txt", "w");

    while(true)
    {
        double start = now_ms();
        struct World* world = CreateWorld();
        struct Camera* camera = CreateCamera();

        for (int x = 0; x < 800; x++)
        {
            for (int y = 0; y < 1000; y++)
            {
                struct Color color = DrawRay(x-400, y-500, world, camera, fptr);

                SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(4);
        if (fptr != NULL) {
            fclose(fptr);
            fptr = NULL;
        }

        free(world);
        free(camera);

        double end = now_ms();
        printf("Frame took %.3f ms (%.1f FPS)\n", end - start, 1000.0 / (end - start));
    }

}

// camera orgin
// camrea width, camera height


// sphere location, sphere raidus
