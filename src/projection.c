#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <threads.h>
#include <raylib.h>
#include <raymath.h>
#include "projection.h"
#include "map.h"
#include "player.h"
#include "common.h"
#include "gui.h"

float calcVFovRad(float hFovRad, RenderConfig* rendConf) {
    return 2 * atanf(tanf(hFovRad/2) * (float)rendConf->height / (float)rendConf->width);
}

void projTerrainSlice(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x) {
    int minY = rendConf->height;
    LineScan lnSc = lnScanCreate(rendConf, cam, map->detailLevels, x);

    float sinVRot = sinf(cam->player->vRotRad);
    float cosVRot = cosf(cam->player->vRotRad);
    float tanHalfVFov = tanf(cam->vFovRad/2);

    while (lnScanAdvance(&lnSc)) {
        Cell cell = mapGetCell(map, lnSc.pos, lnSc.undetailLevel);
        Vector2 relPos = (Vector2) { lnSc.perpDist, cell.height - cam->player->altitude };
        Vector2 rotPos = (Vector2) {
            relPos.x * cosVRot + relPos.y * sinVRot,
            relPos.x * -sinVRot + relPos.y * cosVRot
        };
        int scrY = (int)roundf((0.5f - rotPos.y / 2 / lnSc.perpDist / tanHalfVFov) * rendConf->height);
        scrY = CLAMP(scrY, 0, rendConf->height);

        if (lnSc.perpDist > rendConf->fogStartDist) {
            float fogPercent = (lnSc.perpDist - rendConf->fogStartDist) / (rendConf->viewDist - rendConf->fogStartDist);
            cell.color = ColorLerp(cell.color, map->skyColor, fogPercent);
        }

        if (scrY < minY) {
            ImageDrawLine(&image, x, scrY, x, minY, cell.color);
            minY = scrY;
        }

        if (scrY == 0)
            return;
    }

    ImageDrawLine(&image, x, 0, x, minY, map->skyColor);
}

void projTerrainSlices(Image image, RenderConfig* rendConf, Cam* cam, Map* map, int x0, int width) {
    for (int xOff = 0; xOff < width; xOff++) {
        projTerrainSlice(image, rendConf, cam, map, x0 + xOff);
    }
}

void projTerrainSlicesEntry(DrawTerrSlicesArgs* args) {
    projTerrainSlices(args->image, args->rendConf, args->cam, args->map, args->x0, args->width);
    free(args);
}

void projTerrain(Image image, RenderConfig* rendConf, Cam* cam, Map* map) {
    thrd_t threads[MAX_THREAD_COUNT];
    size_t threadCount = rendConf->threadCount >= MAX_THREAD_COUNT ? MAX_THREAD_COUNT : rendConf->threadCount;
    int widthPerThread = (int)ceilf(rendConf->width / (float)threadCount);

    for (size_t i = 0; i < threadCount; i++) {
        DrawTerrSlicesArgs* args = malloc(sizeof(DrawTerrSlicesArgs));
        *args = (DrawTerrSlicesArgs) {
            .image = image,
            .rendConf = rendConf,
            .cam = cam,
            .map = map,
            .x0 = widthPerThread * (int)i,
            .width = widthPerThread
        };
        thrd_create(&threads[i], (thrd_start_t)projTerrainSlicesEntry, args);
    }

    for (size_t i = 0; i < threadCount; i++) {
        thrd_join(threads[i], NULL);
    }
}
