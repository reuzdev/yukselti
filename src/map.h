#ifndef MAP_H
#define MAP_H

#include <raylib.h>
#include <stddef.h>

#define MAPS_FOLDER_SUBPATH "assets/maps/"
#define COLORMAP_PATH_SUFFIX "/color_map.png"
#define HEIGHTMAP_PATH_SUFFIX "/height_map.png"

typedef struct RenderConfig RenderConfig;
typedef struct Cam Cam;

typedef struct Map {
    Texture colorMapTxt;
    Image* colorMaps;
    Image* elevMaps;
    Vector2 size;
    float maxHeight;
    bool wraps;
    size_t detailLevels;
    Color skyColor;
} Map;

typedef struct Cell {
    Color color;
    float height;
} Cell;

typedef struct LineScan {
    Vector2 pos;
    Vector2 step;
    float perpDist;
    float maxPerpDist;
    float perpStep;
    float maxDistPerLod;
    float maxDistPerLodMultiplier;
    float currPerpDistInLod;
    size_t undetailLevel;
    size_t maxUndetailLevel;
} LineScan;

LineScan lnScanCreate(RenderConfig* rendConf, Cam* cam, size_t maxUndetailLevel, int scrX);
bool lnScanAdvance(LineScan* lnSc);
bool loadMapImages(const char* folderName, Image* colormap, Image* heightmap);
char** getAvailableMaps(size_t* count);
Map mapCreate(Image colormap, Image heightmap, bool wraps, Color skyColor, float maxHeight, size_t detailLevels);
void mapDestroy(Map* map);
Cell mapGetCell(Map* map, Vector2 pos, size_t undetailLevel);

#endif // MAP_H
