#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include "app.h"
#include "map.h"
#include "projection.h"
#include "common.h"

App* appCreate(void) {
    App* a = malloc(sizeof(App));
    if (a == NULL)
        return NULL;

    a->rendConf = (RenderConfig) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight(),
        .fogStartDist = 1300,
        .viewDist = 1500,
        .maxDistPerLod = 500,
        .maxDistPerLodMultiplier = 1,
        .threadCount = 12
    };

    if (!appInitMap(a))
        return NULL;

    a->player = (Player) {
        .pos = Vector2Scale(a->map.size, 0.5f),
        .altitude = a->map.maxHeight + 3,
        .maxMoveSpeed = 500,
        .moveSpeed = 0,
        .maxAscentSpeed = 250,
        .ascentSpeed = 0,
        .accel = 1000,
        .decel = 750,
        .hRotRad = 0,
        .vRotRad = 0,
        .maxHRotSpeedRad = 135 * DEG2RAD,
        .maxVRotSpeedRad = 10 * DEG2RAD,
        .hRotSpeedRad = 0,
        .vRotSpeedRad = 0,
        .hRotAccelRad = 1280 * DEG2RAD,
        .vRotAccelRad = 50 * DEG2RAD
    };

    a->cam = (Cam) {
        .player = &a->player,
        .hFovRad = 75 * DEG2RAD
    };
    a->cam.vFovRad = calcVFovRad(a->cam.hFovRad, &a->rendConf);

    a->terrainImg = (Image) {
        .data = malloc(sizeof(Color) * a->rendConf.width * a->rendConf.height),
        .width = a->rendConf.width,
        .height = a->rendConf.height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    a->terrainTex = LoadTextureFromImage(a->terrainImg);

    memset(a->playerInfo, '\0', sizeof(char) * MENU_DISPLAY_BUFF_SIZE);
    memset(a->fpsLabel, '\0', sizeof(char) * MENU_DISPLAY_BUFF_SIZE);
    a->secSinceFPSUpdated = 0;
    a->framesSinceFPSUpdated = 0;

    appInitSettings(a);
    appInitMenu(a);

    return a;
}

void appDestroy(App* a) {
    UnloadImage(a->terrainImg);
    UnloadTexture(a->terrainTex);
    mapDestroy(&a->map);
    menuDestroy(&a->menu);
    free(a->settings.resSettgDisplay);
    for (size_t i = 0; i < a->settings.mapCount; i++) {
        free(a->settings.mapNames[i]);
    }
    free(a->settings.mapNames);
    free(a);
}

bool appInitMap(App* a) {
    a->settings.mapNames = getAvailableMaps(&a->settings.mapCount);
    if (!a->settings.mapCount) {
        return false;
    }

    Image colormap = { 0 };
    Image heightmap = { 0 };
    bool didLoadMap = false;
    for (size_t i = 0; i < a->settings.mapCount; i++) {
        if (loadMapImages(a->settings.mapNames[i], &colormap, &heightmap)) {
            didLoadMap = true;
            a->settings.mapIdxApplied = (int)i;
            break;
        }
    }

    if (!didLoadMap) {
        return false;
    }
    a->map = mapCreate(colormap, heightmap, true, RAYWHITE, 150, 5);

    return true;
}

void appUpdate(App* a) {
    if (IsWindowResized()) {
        a->rendConf.height = a->rendConf.width*GetScreenHeight()/GetScreenWidth();
        a->terrainImg.data = realloc(a->terrainImg.data, sizeof(Color) * a->rendConf.width * a->rendConf.height);
        a->terrainImg.height = a->rendConf.height;
        UnloadTexture(a->terrainTex);
        a->terrainTex = LoadTextureFromImage(a->terrainImg);
        a->cam.vFovRad = calcVFovRad(a->cam.hFovRad, &a->rendConf);
    }

    UpdateTexture(a->terrainTex, a->terrainImg.data);
    projTerrain(a->terrainImg, &a->rendConf, &a->cam, &a->map);
    if (a->settings.showOptions) {
        menuUpdate(&a->menu);
    }
    else {
        playerControl(&a->player, &a->map);
    }
    appUpdateSettings(a);

    a->secSinceFPSUpdated += GetFrameTime();
    a->framesSinceFPSUpdated++;
    if (a->secSinceFPSUpdated >= FPS_UPDATE_PERIOD_SEC) {
        float fps = a->framesSinceFPSUpdated / a->secSinceFPSUpdated;
        float mspf = a->secSinceFPSUpdated * 1000 / a->framesSinceFPSUpdated;
        switch (a->settings.showFPSLevel) {
        case SHOWFPS_SIMPLE:
            snprintf(a->fpsLabel, MENU_DISPLAY_BUFF_SIZE, "FPS %.2f", fps);
            break;
        case SHOWFPS_DETAILED:
            snprintf(a->fpsLabel, MENU_DISPLAY_BUFF_SIZE, "FPS %.2f\n%.2f ms", fps, mspf);
            break;
        default:
            break;
        }
        a->secSinceFPSUpdated = 0;
        a->framesSinceFPSUpdated = 0;
    }

    if (a->settings.showPlayerInfo) {
        snprintf(
            a->playerInfo, MENU_DISPLAY_BUFF_SIZE,
            "Pos: %.0f, %.0f\nAltitude: %.0f\nSpeed: %.1f",
            a->player.pos.x, a->player.pos.y, a->player.altitude, a->player.moveSpeed
        );
    }
}

void appDraw(App* a) {
    ClearBackground(BLACK);
    DrawTexturePro(
        a->terrainTex,
        (Rectangle) {
            .width = (float)a->terrainTex.width,
            .height = (float)a->terrainTex.height,
            .x = 0,
            .y = 0,
        },
        (Rectangle) {
            .width = (float)GetScreenWidth(),
            .height = (float)GetScreenHeight(),
            .x = 0,
            .y = 0
        },
        Vector2Zero(),
        0,
        WHITE
    );

    Font font = GetFontDefault();
    int lnSpacing = (int)(REL_LINE_SPACING * FONT_SIZE);
    SetTextLineSpacing(lnSpacing);

    float topOff = PADDING;
    if (a->settings.showFPSLevel != SHOWFPS_NONE) {
        Sides fpsSides = sidesCreate(optFltVal(topOff), optFltVal(PADDING), optFltNull(), optFltNull());
        topOff += guiTextWithBG(
            font, a->fpsLabel, fpsSides, PADDING, FONT_SIZE,
            COLOR_BACKGROUND, COLOR_MAIN_TEXT
        ).height;
        topOff += PADDING;
    }
    if (a->settings.showMinimap) {
        Sides minimapSides = sidesCreate(optFltVal(topOff), optFltVal(PADDING), optFltNull(), optFltNull());
        guiMinimap(
            &a->player, &a->map, minimapSides, PADDING,
            MINIMAP_UNIT_SIZE, COLOR_BACKGROUND, COLOR_FOREGROUND
        );
    }

    float leftOff = PADDING;
    if (a->settings.showOptions) {
        Sides menuSides = sidesCreate(optFltVal(PADDING), optFltNull(), optFltNull(), optFltVal(leftOff));
        leftOff += guiMenu(
            font, &a->menu, menuSides, PADDING, FONT_SIZE, REL_LINE_SPACING,
            COLOR_BACKGROUND, COLOR_HIGHLIGHTED_TEXT, COLOR_MAIN_TEXT, COLOR_SECONDARY_TEXT
        ).width;
        leftOff += PADDING;

        MenuItem* selected = a->menu.items[a->menu.selected];
        Sides detailsSides = sidesCreate(optFltVal(PADDING), optFltNull(), optFltNull(), optFltVal(leftOff));
        if (a->menu.items[a->menu.selected]->type == MENU_SETTING) {
            guiSettingDetails(
                font, selected, detailsSides, PADDING, FONT_SIZE,
                REL_LINE_SPACING, COLOR_BACKGROUND, COLOR_MAIN_TEXT
            );
        }
        if (a->menu.items[a->menu.selected]->type == MENU_DESCRIPTION) {
            guiTextWithBG(
                font, selected->desc, detailsSides, PADDING,
                FONT_SIZE, COLOR_BACKGROUND, COLOR_MAIN_TEXT
            );
        }
    }

    float rightOff = PADDING;
    if (a->settings.showControls) {
        Sides controlsSides = sidesCreate(optFltNull(), optFltVal(rightOff), optFltVal(PADDING), optFltNull());
        rightOff += guiTextWithBG(
            font, GENERAL_CONTROLS_LABEL, controlsSides,
            PADDING, FONT_SIZE, COLOR_BACKGROUND, COLOR_MAIN_TEXT
        ).width;
        rightOff += PADDING;
    }
    if (a->settings.showPlayerInfo) {
        Sides playerInfoSides = sidesCreate(optFltNull(), optFltVal(rightOff), optFltVal(PADDING), optFltNull());
        guiTextWithBG(
            font, a->playerInfo, playerInfoSides, PADDING,
            FONT_SIZE, COLOR_BACKGROUND, COLOR_MAIN_TEXT
        );
    }
}

void appInitSettings(App* a) {
    a->settings.resSettgDisplay = calloc(MENU_DISPLAY_BUFF_SIZE, sizeof(char)),
    a->settings.resIdxPeek = INITIAL_RENDER_WIDTH_PRESET,
    a->settings.resIdxTried = INITIAL_RENDER_WIDTH_PRESET,
    a->settings.resIdxApplied = INITIAL_RENDER_WIDTH_PRESET,

    a->settings.mapSettgDisplay = a->settings.mapNames[a->settings.mapIdxApplied];
    a->settings.mapIdxPeek = a->settings.mapIdxApplied;
    a->settings.mapIdxTried = a->settings.mapIdxApplied;

    a->settings.showFPSLevel = SHOWFPS_SIMPLE;
    a->settings.fpsSettgDisplay = (char*)showFPSDisplays[SHOWFPS_SIMPLE];

    a->settings.skyColorIdx = 0;
    a->settings.skySettgDisplay = (char*)colorPresets[0].name;

    a->settings.fogDistPercent = (1 - a->rendConf.fogStartDist/a->rendConf.viewDist) * 100;
    a->settings.hFovDeg = a->cam.hFovRad * RAD2DEG;

    a->settings.threadCount = (float)a->rendConf.threadCount;
    a->settings.showMinimap = true;
    a->settings.showControls = true;
    a->settings.showPlayerInfo = false;
    a->settings.showOptions = false;

    appUpdateSettings(a);
}

void appInitMenu(App* a) {
    MenuItem* tempItems[] = {
        menuItCreateTitle("OPTIONS"),
        menuItCreateTitle("Interface"),
        menuItCreateSetting(settingCreateChoice(
            "Show FPS",
            PERFIMP_NONE,
            "How much detail about\nFPS should be shown",
            &a->settings.showFPSLevel,
            &a->settings.showFPSLevel,
            SHOWFPS_COUNT - 1,
            false,
            &a->settings.fpsSettgDisplay
        )),
        menuItCreateSetting(settingCreateCheckbox(
            "Show Minimap",
            PERFIMP_NONE,
            "Show or hide minimap",
            &a->settings.showMinimap
        )),
        menuItCreateSetting(settingCreateCheckbox(
            "Show Player Info",
            PERFIMP_NONE,
            "Show or hide information\n"
            "about the player's position,\n"
            "altitude etc.",
            &a->settings.showPlayerInfo
        )),
        menuItCreateSetting(settingCreateCheckbox(
            "Show Controls",
            PERFIMP_NONE,
            "Show or hide controls",
            &a->settings.showControls
        )),
        menuItCreateTitle("Environment"),
        menuItCreateSetting(settingCreateChoice(
            "Choose Map",
            PERFIMP_NONE,
            "Select a map from maps folder",
            &a->settings.mapIdxPeek,
            &a->settings.mapIdxTried,
            (int)a->settings.mapCount - 1,
            true,
            &a->settings.mapSettgDisplay
        )),
        menuItCreateSetting(settingCreateSlider(
            "Max Height",
            PERFIMP_NONE,
            "Maximum height represented\nin the heightmap image",
            &a->map.maxHeight,
            10,
            2000,
            10
        )),
        menuItCreateSetting(settingCreateChoice(
            "Sky Color",
            PERFIMP_NONE,
            "Color of the sky",
            &a->settings.skyColorIdx,
            &a->settings.skyColorIdx,
            COLOR_PRESET_COUNT - 1,
            false,
            &a->settings.skySettgDisplay
        )),
        menuItCreateSetting(settingCreateSlider(
            "Fog Distance",
            PERFIMP_NONE,
            "Fog distance as percentage\nof the view distance",
            &a->settings.fogDistPercent,
            0,
            100,
            5
        )),
        menuItCreateTitle("Visual"),
        menuItCreateSetting(settingCreateSlider(
            "Field of View",
            PERFIMP_LOW,
            "Horizontal field of fiev",
            &a->settings.hFovDeg,
            60,
            120,
            5
        )),
        menuItCreateSetting(settingCreateSlider(
            "HQ Distance",
            PERFIMP_HIGH,
            "The distance at which the\nterrain will appear in\nthe highest quality",
            &a->rendConf.maxDistPerLod,
            100,
            2000,
            100
        )),
        menuItCreateSetting(settingCreateSlider(
            "View Distance",
            PERFIMP_EXTREME,
            "The maximum distance at the\nterrain will be visible",
            &a->rendConf.viewDist,
            100,
            10000,
            100
        )),
        menuItCreateSetting(settingCreateChoice(
            "Resolution",
            PERFIMP_EXTREME,
            "Resolution to render the terrain",
            &a->settings.resIdxPeek,
            &a->settings.resIdxTried,
            RENDER_WIDTH_PRESET_COUNT - 1 + 1,
            true,
            &a->settings.resSettgDisplay
        )),
        menuItCreateSetting(settingCreateSlider(
            "Thread Count",
            PERFIMP_EXTREME,
            "Threads to use during\nthe rendering process",
            &a->settings.threadCount,
            1,
            MAX_THREAD_COUNT,
            1
        )),
        menuItCreateDescription(
            "About",
            "Made by reuzdev\nin Izmir, Turkey\nin May 2025\ngithub:\nreuzdev/yukselti"
        ),
        menuItCreateTitle(NAV_UPDOWN_CONTROL_LABEL)
    };

    size_t itemCount = sizeof(tempItems)/sizeof(tempItems[0]);
    MenuItem** items = malloc(itemCount * sizeof(MenuItem*));
    memcpy(items, tempItems, itemCount * sizeof(MenuItem*));
    a->menu = (Menu) {
        .count = itemCount,
        .items = items,
        .selected = 2,
        .selectionPrefix = "> "
    };
}

void appUpdateSettings(App* a) {
    Settings* s = &a->settings;

    if (IsKeyPressed(KEY_SETTINGS))
        s->showOptions = !s->showOptions;

    if (!FloatEquals(a->cam.hFovRad, DEG2RAD * s->hFovDeg)) {
        a->cam.hFovRad = DEG2RAD * s->hFovDeg;
        a->cam.vFovRad = calcVFovRad(a->cam.hFovRad, &a->rendConf);
    }
    a->rendConf.threadCount = (int)s->threadCount;
    a->rendConf.fogStartDist = s->fogDistPercent/100 * a->rendConf.viewDist;
    a->map.skyColor = colorPresets[s->skyColorIdx].color;
    s->mapSettgDisplay = s->mapNames[s->mapIdxPeek];
    s->fpsSettgDisplay = (char*)showFPSDisplays[s->showFPSLevel];
    s->skySettgDisplay = (char*)colorPresets[s->skyColorIdx].name;

    int peekW = s->resIdxPeek == RENDER_WIDTH_PRESET_COUNT ? GetScreenWidth()
                : renderWidthPresets[s->resIdxPeek];
    int peekH = peekW*GetScreenHeight()/GetScreenWidth();
    snprintf(s->resSettgDisplay, MENU_DISPLAY_BUFF_SIZE, "%dx%d", peekW, peekH);

    if (s->resIdxTried != s->resIdxApplied) {
        s->resIdxApplied = s->resIdxTried;
        int targetW = s->resIdxPeek == RENDER_WIDTH_PRESET_COUNT ? GetScreenWidth()
                    : renderWidthPresets[s->resIdxApplied];
        int targetH = targetW*GetScreenHeight()/GetScreenWidth();
        a->rendConf.width = targetW;
        a->rendConf.height = targetH;

        a->terrainImg.width = targetW;
        a->terrainImg.height = targetH;
        a->terrainImg.data = realloc(a->terrainImg.data, sizeof(Color) * targetW * targetH);
        UnloadTexture(a->terrainTex);
        a->terrainTex = LoadTextureFromImage(a->terrainImg);
    }


    if (s->mapIdxTried != s->mapIdxApplied) {
        Image colormap, heightmap;
        if (loadMapImages(s->mapNames[s->mapIdxTried], &colormap, &heightmap)) {
            a->map = mapCreate(colormap, heightmap, a->map.wraps, a->map.skyColor, a->map.maxHeight, a->map.detailLevels);
            s->mapIdxApplied = s->mapIdxTried;
        }
        else {
            s->mapIdxTried = s->mapIdxApplied;
        }
    }
}
