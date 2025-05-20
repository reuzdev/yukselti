#ifndef GUI_H
#define GUI_H

#include "common.h"

typedef enum PerfImpLabelType {
    PERFIMP_NONE = 0,
    PERFIMP_LOW,
    PERFIMP_MEDIUM,
    PERFIMP_HIGH,
    PERFIMP_EXTREME,
    PERFIMP_COUNT
} PerfImpLabelType;

typedef struct PerfImpLabel {
    const char* label;
    Color color;
} PerfImpLabel;

typedef struct OptionalFloat {
    bool exists;
    float value;
} OptionalFloat;

typedef struct Sides {
    OptionalFloat top, right, bottom, left;
} Sides;

typedef enum MenuItemType {
    MENU_SETTING,
    MENU_DESCRIPTION,
    MENU_TITLE
} MenuItemType;

typedef enum SettingType {
    SETTG_SLIDER,
    SETTG_CHECKBOX,
    SETTG_CHOICE
} SettingType;

typedef struct Setting {
    SettingType type;
    PerfImpLabelType perfImpact;
    const char* label;
    const char* description;
    union
    {
        struct {
            float* value;
            float min, max, step;
        } slider;
        struct {
            bool* value;
        } checkbox;
        struct {
            int* peek;
            int* select;
            int max;
            bool askToApply;
            const char** show;
        } choice;
    };
} Setting;

typedef struct MenuItem {
    MenuItemType type;
    union {
        Setting setting;
        const char* desc;
    };
    char display[MENU_DISPLAY_BUFF_SIZE];
} MenuItem;

typedef struct Menu {
    MenuItem** items;
    size_t count;
    size_t selected;
    const char* selectionPrefix;
} Menu;

PerfImpLabel getPerfImpLabel(size_t i);

Rectangle rectWithSizePosDef(Vector2 size, Sides posDef);
Rectangle padRect(Rectangle rect, float padding);
Vector2 rectSize(Rectangle rect);
Vector2 rectPos(Rectangle rect);

OptionalFloat optFltVal(float value);
OptionalFloat optFltNull();
Sides sidesCreate(OptionalFloat top, OptionalFloat right, OptionalFloat bottom, OptionalFloat left);

Setting settingCreateSlider(const char* label, PerfImpLabelType perfImpact, const char* description, float* value, float min, float max, float step);
Setting settingCreateCheckbox(const char* label, PerfImpLabelType perfImpact, const char* description, bool* value);
Setting settingCreateChoice(const char* label, PerfImpLabelType perfImpact, const char* description, int* peek, int* select, int max, bool askToApply, const char** show);
void settingUpdate(Setting* settg);

void menuDestroy(Menu* menu);
void menuUpdate(Menu* menu);
Vector2 menuCalcSize(Menu* menu, float margin, Font font, float fontSz, float relLnSpacing, float* prefixWidth);

MenuItem* menuItCreateTitle(const char* label);
MenuItem* menuItCreateSetting(Setting setting);
MenuItem* menuItCreateDescription(const char* label, const char* description);
void menuItemUpdateDisplay(MenuItem* item);

Rectangle guiMenu(Font font, Menu* menu, Sides pos, float padding, float fontSz, float relLnSpacing, Color bgColor, Color selectColor, Color titleColor, Color normalColor);
Rectangle guiSettingDetails(Font font, MenuItem* item, Sides pos, float padding, float fontSz, float relLnSpacing, Color bgColor, Color normalColor);
Rectangle guiTextWithBG(Font font, const char* string, Sides pos, float padding, float fontSz, Color bgColor, Color fgColor);
Rectangle guiMinimap(Player* player, Map* map, Sides pos, float padding, float unitSize, Color bgColor, Color fgColor);

#endif // GUI_H
