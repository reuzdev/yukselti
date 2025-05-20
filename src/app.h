#ifndef APP_H
#define APP_H

#include "player.h"
#include "projection.h"
#include "map.h"
#include "gui.h"

typedef enum FPSSettingValues {
    FPS_SETTG_DISABLED = 0,
    FPS_SETTG_SIMPLE,
    FPS_SETTG_DETAILED,
    FPS_SETTG_COUNT
} FPSSettingValues;

typedef struct SkyPreset {
    const char* name;
    Color color;
} SkyPreset;

typedef struct Settings {
    const int* rendWidths;
    int rendWidthCount;
    char* rendWSettgDisplay;
    int rendWIdxPeek, rendWIdxTried, rendWIdxApplied;

    char* mapSettgDisplay;
    char** mapNames;
    int mapIdxPeek, mapIdxTried, mapIdxApplied;
    size_t mapCount;

    const char** fpsSettgDisplays;
    char* fpsSettgDisplay;
    int fpsSettg;

    const SkyPreset* skyPresets;
    int skyPresetCount;
    const char** skySettgDisplays;
    char* skySettgDisplay;
    int skySettg;

    float fogDistPercent;
    float hFovDeg;

    float threadCount;
    bool showMinimap, showPlayerInfo, showControls, showOptions;
} Settings;

typedef struct App {
    Player player;
    Map map;
    RenderConfig rendConf;
    Cam cam;
    Image terrainImg;
    Texture terrainTex;
    char fpsLabel[MENU_DISPLAY_BUFF_SIZE];
    char playerInfo[MENU_DISPLAY_BUFF_SIZE];
    float secSinceFPSUpdated;
    size_t framesSinceFPSUpdated;
    Menu menu;
    Settings settings;
} App;

App* appCreate(void);
void appDestroy(App* a);
void appUpdate(App* a);
void appDraw(App* a);

bool appInitMap(App* a);
void appInitSettings(App* a);
void appInitMenu(App* a);
void appUpdateSettings(App* a);

#endif // APP_H
