#ifndef PROJECTION_H
#define PROJECTION_H

#include <raylib.h>

typedef struct Player Player;
typedef struct Map Map;
typedef struct Menu Menu;
typedef struct Sides Sides;
typedef struct MenuItem MenuItem;

typedef struct Cam {
    Player* player;
    float hFovRad;
    float vFovRad;
} Cam;

typedef struct RenderConfig {
    int width;
    int height;
    float viewDist;
    float fogStartDist;
    float maxDistPerLod;
    float maxDistPerLodMultiplier;
    int threadCount;
} RenderConfig;

typedef struct ProjTerrSlicesArgs {
    Image image;
    RenderConfig* rendConf;
    Cam* cam;
    Map* map;
    int x0;
    int width;
} ProjTerrSlicesArgs;

float calcVFovRad(float hFovRad, RenderConfig* rendConf);
void projTerrainSlice(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x);
void projTerrainSlices(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x0, int width);
int projTerrainSlicesEntry(void* args_);
void projTerrain(Image image, RenderConfig* rendConf, Cam* cam, Map* map);

#endif // PROJECTION_H
