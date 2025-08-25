#include <float.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "VectorMath.c"

struct Color
{
    Uint8 R;
    Uint8 G;
    Uint8 B;
    Uint8 A;
};

struct Sphere
{
    // Radius of the sphere
    float R;

    // Location of the center of the sphere
    struct Vector3 Center;

    struct Color Color;
};

struct Plane
{
    // todo: Ensure normalized
    struct Vector3 Normal;

    struct Vector3 Location;

    float Size;

    struct Color Color;
};

enum ObjectType {
    Sphere,
    Plane,
};

union ObjectData
{
    struct Sphere Sphere;
    struct Plane Plane;
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
    enum ObjectType ObjectTypes[10];
    union ObjectData ObjectData[10];
    int NumObjects;

    enum LightType LightTypes[10];
    union LightSource LightSources[10];
    int NumLightSources;
};

struct Camera
{
    // Actual location of the camera where the rays start
    struct Vector3 Location;

    // Distance of the screen from the camera location
    float ScreenDistance;
};

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

// returns the closest intersection point
int GetIntersection(struct Vector3 ray, struct World* world, struct Camera* camera, struct Vector3* HitLocation, struct Vector3* Normal, struct Color* Color)
{
    // How much to multiply ray by to get the point. We aim for the smallest value.
    ray = VectNormalize(ray);

    int found = 0;
    float smallestRayMultiple = FLT_MAX;

    struct Vector3 TempHitLocation;
    struct Vector3 TempNormal;
    struct Color TempColor;

    for (int i=0; i<world->NumObjects; i++)
    {
        switch (world->ObjectTypes[i])
        {
            case Sphere:
                struct Sphere* sphere = &world->ObjectData[i].Sphere;
                struct Vector3 CO = VectSubtract(camera->Location, sphere->Center);

                float a = VectDot(ray, ray);
                float b = 2 * VectDot(CO, ray);
                float c = VectDot(CO, CO) - (sphere->R * sphere->R);

                // direction vector times this will get you the point on the sphere
                float res = QuadraticSolveSml(a,b,c);
                if (res == -1)
                    continue;

                if (res >= smallestRayMultiple)
                    continue;

                TempHitLocation = VectAdd(camera->Location, VectMult(ray, res));
                TempNormal = VectNormalize(VectSubtract(TempHitLocation, sphere->Center));
                TempColor = sphere->Color;
                smallestRayMultiple = res;
                found = 1;

                break;
            case Plane:
                struct Plane* plane = &world->ObjectData[i].Plane;
                // If this the dot of these is 0, that means the ray will NEVER intersect with the plane
                // (or infinitely intersects)

                // if negative that means the ray is going towards the normal basically.
                float check = VectDot(ray, plane->Normal);
                if (check >= -0.0005f)
                    continue;

                float numerator = VectDot(VectSubtract(plane->Location, camera->Location), plane->Normal);
                float solution = numerator / check;

                if (solution >= smallestRayMultiple)
                    continue;

                struct Vector3 Hifff = VectAdd(camera->Location, VectMult(ray, solution));
                struct Vector3 Delta = VectSubtract(Hifff, plane->Location);
                // if (VectLength(Delta) >= plane->Size)
                //     continue;

                // TOOD: Do distance checks...

                TempHitLocation = VectAdd(camera->Location, VectMult(ray, solution));
                TempNormal = plane->Normal;
                TempColor = plane->Color;
                smallestRayMultiple = solution;
                found = 1;

                break;
        }
    }

    if (found == 0)
    {
        HitLocation = NULL;
        Normal = NULL;
        Color = NULL;
        return 0;
    }

    *HitLocation = TempHitLocation;
    *Normal = TempNormal;
    *Color = TempColor;
    return 1;
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
            case PointLight:
                struct PointLight* pointLight = &world->LightSources[i].PointLight;

                struct Vector3 LightDirection = VectNormalize(VectSubtract(pointLight->Location, point));
                // light dot normal
                float ldna = VectDot(LightDirection, normal);

                // printf("%f\n", ldna);

                if (ldna <= 0)
                    continue;
                
                intensity += ldna * pointLight->Intensity;
                break;
            case DirectionalLight:
                struct DirectionalLight* directionalLight = &world->LightSources[i].DirectionalLight;

                // struct Vector3 LightDirection = VectNormalize(directionalLight->Direction);
                // todo: assert directional light is normalized

                // light dot normal
                float ldnb = VectDot(directionalLight->Direction, normal);
                if (ldnb <= 0)
                    continue;

                intensity += ldnb * directionalLight->Intensity;

                break;
            case AmbientLight:
                struct AmbientLight* ambientLight = &world->LightSources[i].AmbientLight;
                intensity += ambientLight->Intensity;
                break;
        }
    }

    return intensity;
}

//x,y point on the camera display
struct Color DrawRay(int x, int y, struct World* world, struct Camera* camera, FILE* fileptr)
{
    struct Vector3 HitLocation;
    struct Vector3 Normal;
    struct Color Color;

    struct Vector3 direction = VectSubtract((struct Vector3){x,y,camera->ScreenDistance}, camera->Location);
    int found = GetIntersection(direction, world, camera, &HitLocation, &Normal, &Color);

    if (found == 0)
        return (struct Color){0,0,0, 255};


    // Write some text to the file
    if (fileptr != NULL && x%100==0 && y%100==0)
        fprintf(fileptr, "\\operatorname{vector}((%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f))\n", camera->Location.X, camera->Location.Y, camera->Location.Z, HitLocation.X, HitLocation.Y, HitLocation.Z);
    // fprintf(fptr, "vector((%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f))\n", HitLocation.X, HitLocation.Y, HitLocation.Z, Normal.X, Normal.Y, Normal.Z);
    // fprintf(fptr, "vector((%f,%f,%f),(%f,%f,%f))", HitLocation.X, HitLocation.Y, HitLocation.Z, Normal.X, Normal.Y, Normal.Z);

    // Close the file
    // fclose(fptr);
    // exit(EXIT_SUCCESS);

    // light intensity at that point
    float lightIntensity = GetLightIntensity(HitLocation, Normal, world);

    float r = lightIntensity * Color.R;
    float g = lightIntensity * Color.G;
    float bb = lightIntensity * Color.B;
    r = r>255 ? 255 : r;
    g = g>255 ? 255 : g;
    bb = bb>255 ? 255 : bb;

    return (struct Color){(Uint8)r,(Uint8)g,(Uint8)bb, 255};
}

void CreateObjects(struct World* world)
{
    world->NumObjects = 1;
    world->ObjectTypes[0] = Sphere;
    world->ObjectData[0].Sphere = (struct Sphere){
        100,
        (struct Vector3){ 350, 350, 1320 },
        (struct Color){ 254, 10, 10, 255 },
    };

    // world->NumObjects = 2;
    // world->ObjectTypes[1] = Plane;
    // world->ObjectData[1].Plane = (struct Plane){
    //     (struct Vector3){ 0, -1, 0 },
    //     (struct Vector3){ 10, 10, 1320 },
    //     100,
    //     (struct Color){ 10, 254, 10, 255 },
    // };

    world->NumObjects = 2;
    world->ObjectTypes[1] = Plane;
    world->ObjectData[1].Plane = (struct Plane){
        (struct Vector3){ -1, 0, 0 },
        (struct Vector3){ 10, 10, 1400 },
        10000,
        (struct Color){ 10, 10, 254, 255 },
    };
}

void CreateLights(struct World* world)
{
    world->NumLightSources = 1;
    world->LightTypes[0] = PointLight;

    int xPos;
    int yPos;

    SDL_PumpEvents();
    Uint32 state = SDL_GetMouseState(&xPos, &yPos);

    if (state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        exit(0);
    }

    world->LightSources[0].PointLight = (struct PointLight){(struct Vector3){xPos,yPos,1500}, 20};
    // printf("Mouse Position: %d, %d\n", xPos, yPos);

    world->NumLightSources = 2;
    world->LightTypes[1] = AmbientLight;

    world->LightSources[1].AmbientLight = (struct AmbientLight){2.5};
}

struct World* CreateWorld()
{
    struct World* world = malloc(sizeof(struct World));

    CreateObjects(world);
    CreateLights(world);

    return world;
}

struct Camera* CreateCamera()
{
    struct Camera* camera = malloc(sizeof(struct Camera));
    camera->Location = (struct Vector3){ 320,320,0 };
    camera->ScreenDistance = 1150;
    return camera;
}

int main() 
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 800, 0, &window, &renderer);
    SDL_RenderSetScale(renderer,1,1);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen("out.txt", "w");

    while(true)
    {
        struct World* world = CreateWorld();
        struct Camera* camera = CreateCamera();

        for (int x = 0; x < 800; x++)
        {
            for (int y = 0; y < 800; y++)
            {
                struct Color color = DrawRay(x, y, world, camera, fptr);

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
    }

}

// camera orgin
// camrea width, camera height


// sphere location, sphere raidus