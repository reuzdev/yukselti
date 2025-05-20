#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>
#include <stddef.h>

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
#define VROT_HALF_RANGE         (40 * DEG2RAD)

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

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(n, min, max) (MAX((min), MIN((n), (max))))

char* concatStrings(size_t count, ...);

#endif // COMMON_H
