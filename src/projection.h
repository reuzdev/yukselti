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

typedef struct DrawTerrSlicesArgs {
    Image image;
    RenderConfig* rendConf;
    Cam* cam;
    Map* map;
    int x0;
    int width;
} DrawTerrSlicesArgs;

float calcVFovRad(float hFovRad, RenderConfig* rendConf);
void projTerrainSlice(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x);
void projTerrainSlices(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x0, int width);
void projTerrainSlicesEntry(DrawTerrSlicesArgs* args);
void projTerrain(Image image, RenderConfig* rendConf, Cam* cam, Map* map);

#endif // PROJECTION_H
