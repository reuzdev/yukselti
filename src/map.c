#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>
#include "projection.h"
#include "player.h"
#include "map.h"
#include "common.h"

LineScan lnScanCreate(RenderConfig* rendConf, Cam* cam, size_t maxUndetailLevel, int scrX) {
    float distToScreen = rendConf->width / 2.0f / tanf(cam->hFovRad/2);
    float angleDelta = atanf((rendConf->width / 2.0f - scrX) / distToScreen);
    float angle = cam->player->hRotRad + angleDelta;

    float absCos = fabsf(cosf(cam->player->hRotRad));
    float absSin = fabsf(sinf(cam->player->hRotRad));
    float perpStep = 1 / (absCos > absSin ? absCos : absSin);
    float linearStep = perpStep / cosf(angleDelta);
    Vector2 step = (Vector2) {
        linearStep * cosf(angle),
        linearStep * -sinf(angle)
    };

    return (LineScan) {
        .pos = cam->player->pos,
        .step = step,
        .perpDist = 0,
        .maxPerpDist = rendConf->viewDist,
        .perpStep = perpStep,
        .maxDistPerLod = rendConf->maxDistPerLod,
        .maxDistPerLodMultiplier = rendConf->maxDistPerLodMultiplier,
        .currPerpDistInLod = 0,
        .undetailLevel = 0,
        .maxUndetailLevel = maxUndetailLevel
    };
}

bool lnScanAdvance(LineScan* lnSc) {
    if (lnSc->perpDist > lnSc->maxPerpDist)
        return false;

    lnSc->pos = Vector2Add(lnSc->pos, lnSc->step);
    lnSc->perpDist += lnSc->perpStep;
    lnSc->currPerpDistInLod += lnSc->perpStep;

    if (lnSc->currPerpDistInLod > lnSc->maxDistPerLod && lnSc->undetailLevel < lnSc->maxUndetailLevel) {
        lnSc->step = Vector2Scale(lnSc->step, 2);
        lnSc->perpStep *= 2;
        lnSc->currPerpDistInLod = 0;
        lnSc->maxDistPerLod *= lnSc->maxDistPerLodMultiplier;
        lnSc->undetailLevel++;
    }

    return true;
}

char** getAvailableMaps(size_t* count) {
    char* mapsFolder = concatStrings(2, GetApplicationDirectory(), MAPS_FOLDER_SUBPATH);
    FilePathList filePaths = LoadDirectoryFiles(mapsFolder);
    char** result = malloc(filePaths.count * sizeof(char*));
    *count = 0;

    for (size_t i = 0; i < filePaths.count; i++) {
        bool exists = false;
        const char* raylibFileName = GetFileName(filePaths.paths[i]);
        size_t nameLen = strlen(raylibFileName);
        char* name = malloc((nameLen + 1) * sizeof(char));
        strcpy(name, raylibFileName);
        for (size_t k = 0; k < *count; k++) {
            if (strcmp(result[k], name) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists)
            result[(*count)++] = name;
        else
            free(name);
    }

    UnloadDirectoryFiles(filePaths);

    free(mapsFolder);
    if (*count == 0) {
        free(result);
        return NULL;
    }

    return result;
}

bool loadMapImages(const char* folderName, Image* colormap, Image* heightmap) {
    const char* rootPath = GetApplicationDirectory();
    char* colormapPath = concatStrings(4, rootPath, MAPS_FOLDER_SUBPATH, folderName, COLORMAP_PATH_SUFFIX);
    char* heightmapPath = concatStrings(4, rootPath, MAPS_FOLDER_SUBPATH, folderName, HEIGHTMAP_PATH_SUFFIX);

    bool filesExist = FileExists(colormapPath) || FileExists(heightmapPath);
    if (filesExist) {
        *colormap = LoadImage(colormapPath);
        *heightmap = LoadImage(heightmapPath);
    }

    free(colormapPath);
    free(heightmapPath);
    return filesExist;
}

Map mapCreate(Image colormap, Image heightmap, bool wraps, Color skyColor, float maxHeight, size_t detailLevels) {
    Image* colorMaps = malloc(detailLevels * sizeof(Image));
    Image* elevMaps = malloc(detailLevels * sizeof(Image));

    colorMaps[0] = colormap;
    elevMaps[0] = heightmap;

    for (size_t i = 1; i < detailLevels; i++) {
        colorMaps[i] = ImageCopy(colormap);
        elevMaps[i] = ImageCopy(heightmap);
        ImageResize(&colorMaps[i], colormap.width >> i, colormap.height >> i);
        ImageResize(&elevMaps[i], heightmap.width >> i, heightmap.height >> i);
    }

    return (Map) {
        .colorMapTxt = LoadTextureFromImage(colormap),
        .colorMaps = colorMaps,
        .elevMaps = elevMaps,
        .size = (Vector2) { (float)colormap.width, (float)colormap.height },
        .maxHeight = maxHeight,
        .wraps = wraps,
        .skyColor = skyColor,
        .detailLevels = detailLevels
    };
}

void mapDestroy(Map* map) {
    for (size_t i = 0; i < map->detailLevels; i++) {
        UnloadImage(map->colorMaps[i]);
        UnloadImage(map->elevMaps[i]);
    }
    free(map->colorMaps);
    free(map->elevMaps);
    map->colorMaps = NULL;
    map->elevMaps = NULL;
    UnloadTexture(map->colorMapTxt);
}

Cell mapGetCell(Map* map, Vector2 pos, size_t undetailLevel) {
    if (map->wraps) {
        pos.x = Wrap(pos.x, 0, map->size.x);
        pos.y = Wrap(pos.y, 0, map->size.y);
    }

    if (pos.x < 0 || pos.x >= map->size.x || pos.y < 0 || pos.y >= map->size.y) {
        return (Cell) {
            .color = BLACK,
            .height = 0
        };
    }

    undetailLevel = undetailLevel >= map->detailLevels ? map->detailLevels - 1 : undetailLevel;

    int colorX = (int)(pos.x / map->size.x * map->colorMaps[undetailLevel].width);
    int colorY = (int)(pos.y / map->size.y * map->colorMaps[undetailLevel].height);
    int heightX = (int)(pos.x / map->size.x * map->elevMaps[undetailLevel].width);
    int heightY = (int)(pos.y / map->size.y * map->elevMaps[undetailLevel].height);

    Color color = GetImageColor(map->colorMaps[undetailLevel], colorX, colorY);
    float height = GetImageColor(map->elevMaps[undetailLevel], heightX, heightY).r / 255.0f * map->maxHeight;

    return (Cell) {
        .color = color,
        .height = height
    };
}
