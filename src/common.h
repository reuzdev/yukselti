#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>

#define MAX_THREAD_COUNT        24
#define MENU_DISPLAY_BUFF_SIZE  256
#define FPS_UPDATE_PERIOD_SEC   0.1f

#define COLOR_BACKGROUND        (Color) { 20, 20, 20, 150 }
#define COLOR_FOREGROUND        (Color) { 240, 240, 240, 255 }
#define COLOR_MAIN_TEXT         COLOR_FOREGROUND
#define COLOR_SECONDARY_TEXT    (Color) { 200, 200, 200, 255 }
#define COLOR_HIGHLIGHTED_TEXT  (Color) { 82, 255, 99, 255 }
#define PADDING                 10
#define FONT_SIZE               20
#define REL_LETTER_SPACING      0.1f
#define REL_LINE_SPACING        0.1f
#define MINIMAP_UNIT_SIZE       100
#define VROT_HALF_RANGE         (25 * DEG2RAD)

#define KEY_GO_FORWARD  KEY_UP
#define KEY_GO_BACKWARD KEY_DOWN
#define KEY_TURN_LEFT   KEY_LEFT
#define KEY_TURN_RIGHT  KEY_RIGHT
#define KEY_LOOK_UP     KEY_Q
#define KEY_LOOK_DOWN   KEY_A
#define KEY_FLY_UP      KEY_W
#define KEY_FLY_DOWN    KEY_S
#define KEY_SETTINGS    KEY_ESCAPE
#define KEY_NAV_UP      KEY_UP
#define KEY_NAV_DOWN    KEY_DOWN
#define KEY_NAV_LEFT    KEY_LEFT
#define KEY_NAV_RIGHT   KEY_RIGHT
#define KEY_APPLY       KEY_SPACE

#define NAV_UPDOWN_CONTROL_LABEL      "[UP/DOWN] to navigate"
#define SLIDER_N_CHOICE_CONTROL_LABEL "[LEFT/RIGHT] to change"
#define CHOICE_CONTROL_W_APPLY_LABEL  "[LEFT/RIGHT] to change\n[SPACE] to apply"
#define CHECKBOX_CONTROL_LABEL        "[SPACE] to change"
#define GENERAL_CONTROLS_LABEL        "CONTROLS\n"     \
                                      "[UP/DOWN] move\n"   \
                                      "[LEFT/RIGHT] turn\n"   \
                                      "[Q/A] look up or down\n"  \
                                      "[W/S] fly up or dowm\n"    \
                                      "[ESC] settings"

static const int renderWidthPresets[] = {
    200, 400, 800, 1280, 1920
};

#define INITIAL_RENDER_WIDTH_PRESET 2
#define RENDER_WIDTH_PRESET_COUNT sizeof(renderWidthPresets)/sizeof(renderWidthPresets[0])

typedef struct ColorPreset {
    const char* name;
    Color color;
} ColorPreset;

static const ColorPreset colorPresets[] = {
    { "Blue", { 102, 191, 255, 255 } },
    { "White", { 245, 245, 245, 255 } },
    { "Yellow", { 255, 253, 156, 255 }},
    { "Red", { 200, 20, 13, 255 }},
    { "Orange", { 255, 143, 74, 255 }}
};

#define COLOR_PRESET_COUNT sizeof(colorPresets)/sizeof(colorPresets[0])

typedef enum PerformanceImpact {
    PERFIMP_NONE = 0,
    PERFIMP_LOW,
    PERFIMP_MEDIUM,
    PERFIMP_HIGH,
    PERFIMP_EXTREME,
    PERFIMP_COUNT
} PerformanceImpact;

typedef struct PerformanceImpactPreset {
    const char* label;
    Color color;
} PerformanceImpactPreset;

static const PerformanceImpactPreset perfImpPresets[PERFIMP_COUNT] = {
    [PERFIMP_NONE] = {
        .label = "Performance Impact:\nNEGLIGABLE",
        .color = { 111, 226, 237, 255 }
    },
    [PERFIMP_LOW] = {
        .label = "Performance Impact:\nLOW",
        .color = { 176, 237, 111, 255 }
    },
    [PERFIMP_MEDIUM] = {
        .label = "Performance Impact:\nMEDIUM",
        .color = { 240, 212, 120, 255 }
    },
    [PERFIMP_HIGH] = {
        .label = "Performance Impact:\nHIGH",
        .color = { 255, 99, 138, 255 }
    },
    [PERFIMP_EXTREME] = {
        .label = "Performance Impact:\nEXTREME",
        .color = { 192, 110, 255, 255 }
    }
};

typedef enum ShowFPSLevel {
    SHOWFPS_NONE,
    SHOWFPS_SIMPLE,
    SHOWFPS_DETAILED,
    SHOWFPS_COUNT
} ShowFPSLevel;

static const char* showFPSDisplays[SHOWFPS_COUNT] = {
    [SHOWFPS_NONE] = "Disabled",
    [SHOWFPS_SIMPLE] = "Simple",
    [SHOWFPS_DETAILED] = "Detailed",
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(n, min, max) (MAX((min), MIN((n), (max))))

char* concatStrings(size_t count, ...);

#endif // COMMON_H
