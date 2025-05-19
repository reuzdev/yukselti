#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include "common.h"
#include "player.h"
#include "map.h"
#include "gui.h"

OptionalFloat optFltVal(float value) {
    return (OptionalFloat) {
        .exists = true,
        .value = value
    };
}

OptionalFloat optFltNull() {
    return (OptionalFloat) {
        .exists = false,
        .value = 0
    };
}

Rectangle rectWithSizePosDef(Vector2 size, Sides posDef) {
    Vector2 pos;

    if (posDef.right.exists && posDef.left.exists) {
        pos.x = posDef.right.value;
        size.x = MAX(posDef.left.value - pos.x, size.x);
    }
    else if (posDef.right.exists) {
        pos.x = posDef.right.value;
    }
    else if (posDef.left.exists) {
        pos.x = GetScreenWidth() - posDef.left.value - size.x;
    }
    else {
        pos.x = GetScreenWidth()/2 - size.x/2;
    }

    if (posDef.top.exists && posDef.bottom.exists) {
        pos.y = posDef.top.value;
        size.y = MAX(posDef.bottom.value - pos.y, size.y);
    }
    else if (posDef.top.exists) {
        pos.y = posDef.top.value;
    }
    else if (posDef.bottom.exists) {
        pos.y = GetScreenHeight() - posDef.bottom.value - size.y;
    }
    else {
        pos.y = GetScreenHeight()/2 - size.y/2;
    }

    return (Rectangle) {
        .width = size.x,
        .height = size.y,
        .x = pos.x,
        .y = pos.y
    };
}

Rectangle padRect(Rectangle rect, float padding) {
    return (Rectangle) {
        .width = rect.width - padding * 2,
        .height = rect.height - padding * 2,
        .x = rect.x + padding,
        .y = rect.y + padding
    };
}

Vector2 rectSize(Rectangle rect) {
    return (Vector2) {
        .x = rect.width,
        .y = rect.height
    };
}

Vector2 rectPos(Rectangle rect) {
    return (Vector2) {
        .x = rect.x,
        .y = rect.y
    };
}

Sides sidesCreate(OptionalFloat top, OptionalFloat right, OptionalFloat bottom, OptionalFloat left) {
    return (Sides) {
        .top = top,
        .right = right,
        .bottom = bottom,
        .left = left
    };
}

Setting settingCreateSlider(const char* label, PerformanceImpact perfImpact, const char* description, float* value, float min, float max, float step) {
    return (Setting) {
        .label = label,
        .type = SETTG_SLIDER,
        .perfImpact = perfImpact,
        .description = description,
        .slider.value = value,
        .slider.min = min,
        .slider.max = max,
        .slider.step = step
    };
}

Setting settingCreateCheckbox(const char* label, PerformanceImpact perfImpact, const char* description, bool* value) {
    return (Setting) {
        .label = label,
        .type = SETTG_CHECKBOX,
        .perfImpact = perfImpact,
        .description = description,
        .checkbox.value = value
    };
}

Setting settingCreateChoice(const char* label, PerformanceImpact perfImpact, const char* description, int* peek, int* select, int max, bool askToApply, const char** show) {
    return (Setting) {
        .label = label,
        .type = SETTG_CHOICE,
        .perfImpact = perfImpact,
        .description = description,
        .choice.peek = peek,
        .choice.select = select,
        .choice.max = max,
        .choice.askToApply = askToApply,
        .choice.show = show
    };
}

void menuDestroy(Menu* menu) {
    for (size_t i = 0; i < menu->count; i++) {
        free(menu->items[i]);
    }
    free(menu->items);
}

void menuUpdate(Menu* menu) {
    int navDir = IsKeyPressed(KEY_NAV_DOWN) + IsKeyPressedRepeat(KEY_NAV_DOWN)
            - IsKeyPressed(KEY_NAV_UP) - IsKeyPressedRepeat(KEY_NAV_UP);

    if (navDir) {
        for (int i = (int)menu->selected + navDir; i >= 0 && i < menu->count; i += navDir) {
            if (menu->items[i]->type != MENU_TITLE) {
                menu->selected = i;
                break;
            }
        }
    }

    if (menu->items[menu->selected]->type == MENU_SETTING) {
        settingUpdate(&menu->items[menu->selected]->setting);
        menuItemUpdateDisplay(menu->items[menu->selected]);
    }
}

MenuItem* menuItCreateTitle(const char* label) {
    MenuItem* item = malloc(sizeof(MenuItem));
    *item = (MenuItem) {
        .type = MENU_TITLE,
    };
    strcpy(item->display, label);
    return item;
}

MenuItem* menuItCreateSetting(Setting setting) {
    MenuItem* item = malloc(sizeof(MenuItem));
    *item = (MenuItem) {
        .type = MENU_SETTING,
        .setting = setting
    };
    settingUpdate(&setting);
    menuItemUpdateDisplay(item);
    return item;
}

MenuItem* menuItCreateDescription(const char* label, const char* description) {
    MenuItem* item = malloc(sizeof(MenuItem));
    *item = (MenuItem) {
        .type = MENU_DESCRIPTION,
        .desc = description
    };
    strcpy(item->display, label);
    return item;
}

void settingUpdate(Setting* settg) {
    switch (settg->type)
    {
    case SETTG_CHECKBOX:
        if (IsKeyPressed(KEY_APPLY))
            *settg->checkbox.value = !*settg->checkbox.value;
        break;
    case SETTG_CHOICE:
        if (IsKeyPressed(KEY_NAV_LEFT) || IsKeyPressedRepeat(KEY_NAV_LEFT))
            (*settg->choice.peek)--;
        if (IsKeyPressed(KEY_NAV_RIGHT) || IsKeyPressedRepeat(KEY_NAV_RIGHT))
            (*settg->choice.peek)++;
        *settg->choice.peek = CLAMP(*settg->choice.peek, 0, settg->choice.max);
        *settg->choice.select = CLAMP(*settg->choice.select, 0, settg->choice.max);

        if (!settg->choice.askToApply || IsKeyPressed(KEY_APPLY))
            *settg->choice.select = *settg->choice.peek;
        break;
    case SETTG_SLIDER:
        int changeDir = IsKeyPressed(KEY_NAV_RIGHT) + IsKeyPressedRepeat(KEY_NAV_RIGHT)
                      - IsKeyPressed(KEY_NAV_LEFT) - IsKeyPressedRepeat(KEY_NAV_LEFT);
        *settg->slider.value += settg->slider.step * changeDir;
        *settg->slider.value = Clamp(*settg->slider.value, settg->slider.min, settg->slider.max);
        break;
    }
}

Vector2 menuCalcSize(Menu* menu, float margin, Font font, float fontSz, float relLnSpacing, float* prefixWidth) {
    float letterSpacing = fontSz * REL_LETTER_SPACING;
    float lineSpacing = fontSz * relLnSpacing;
    *prefixWidth = MeasureTextEx(font, menu->selectionPrefix, fontSz, letterSpacing).x + letterSpacing;
    Vector2 bgSize = (Vector2) { *prefixWidth, 0 };

    for (size_t i = 0; i < menu->count; i++) {
        Vector2 size = MeasureTextEx(font, menu->items[i]->display, fontSz, letterSpacing);
        if (menu->items[i]->type != MENU_TITLE) {
            size.x += *prefixWidth;
        }
        bgSize.x = MAX(bgSize.x, size.x);
        bgSize.y += fontSz;
    }

    bgSize.y += menu->count * lineSpacing - lineSpacing;
    return Vector2AddValue(bgSize, margin * 2);
}

void menuItemUpdateDisplay(MenuItem* item) {
    if (item->type != MENU_SETTING)
        return;

    switch (item->setting.type)
    {
    case SETTG_CHECKBOX:
        const char* checkboxSuffix = *item->setting.checkbox.value ? "(X)" : "( )";
        snprintf(item->display, MENU_DISPLAY_BUFF_SIZE, "%s %s", item->setting.label, checkboxSuffix);
        break;
    case SETTG_CHOICE:
        const char* choiceSuffix = *item->setting.choice.peek == *item->setting.choice.select ? "" : "*";
        snprintf(item->display, MENU_DISPLAY_BUFF_SIZE, "%s (%s)%s", item->setting.label, *item->setting.choice.show, choiceSuffix);
        break;
    case SETTG_SLIDER:
        snprintf(item->display, MENU_DISPLAY_BUFF_SIZE, "%s (%.2f)", item->setting.label, *item->setting.slider.value);
        break;
    }
}

Rectangle guiMenu(Font font, Menu* menu, Sides pos, float padding, float fontSz, float relLnSpacing, Color bgColor, Color selectColor, Color titleColor, Color normalColor) {
    float prefixWidth;
    Vector2 bgMinSize = menuCalcSize(menu, padding, font, fontSz, relLnSpacing, &prefixWidth);
    Rectangle bgRect = rectWithSizePosDef(bgMinSize, pos);

    DrawRectangleRec(bgRect, bgColor);
    Vector2 contentTopLeft = Vector2AddValue(rectPos(bgRect), padding);

    float lineSpacing = fontSz * relLnSpacing;
    float letterSpacing = fontSz * REL_LETTER_SPACING;

    for (size_t i = 0; i < menu->count; i++) {
        Vector2 itemPos = (Vector2) {
            contentTopLeft.x,
            contentTopLeft.y + i * (fontSz + lineSpacing)
        };
        Color color = menu->items[i]->type == MENU_TITLE ? titleColor : normalColor;

        if (i == menu->selected) {
            color = selectColor;
            DrawTextEx(font, menu->selectionPrefix, itemPos, fontSz, REL_LETTER_SPACING, color);
            itemPos.x += prefixWidth;
        }

        DrawTextEx(font, menu->items[i]->display, itemPos, fontSz, letterSpacing, color);
    }

    return bgRect;
}

Rectangle guiSettingDetails(Font font, MenuItem* item, Sides pos, float padding, float fontSz, float relLnSpacing, Color bgColor, Color normalColor) {
    PerformanceImpact perfImp = item->setting.perfImpact;
    const char* perfImpLabel = perfImpPresets[perfImp].label;
    float letterSpacing = fontSz * REL_LETTER_SPACING;
    float lineSpacing = fontSz * relLnSpacing;
    char* controlText = NULL;
    SetTextLineSpacing((int)lineSpacing);

    switch (item->setting.type)
    {
    case SETTG_CHECKBOX:
        controlText = CHECKBOX_CONTROL_LABEL;
        break;
    case SETTG_SLIDER:
        controlText = SLIDER_N_CHOICE_CONTROL_LABEL;
        break;
    case SETTG_CHOICE:
        if (item->setting.choice.askToApply) {
            controlText = CHOICE_CONTROL_W_APPLY_LABEL;
        }
        else {
            controlText = SLIDER_N_CHOICE_CONTROL_LABEL;
        }
        break;
    }

    Vector2 titleSize = MeasureTextEx(font, item->display, fontSz, letterSpacing);
    Vector2 descSize = MeasureTextEx(font, item->setting.description, fontSz, letterSpacing);
    Vector2 perfTextSize = MeasureTextEx(font, perfImpLabel, fontSz, letterSpacing);
    Vector2 controlTextSize = MeasureTextEx(font, controlText, fontSz, letterSpacing);
    Vector2 bgMinSize = {
        MAX(titleSize.x, MAX(descSize.x, MAX(perfTextSize.x, controlTextSize.x))) + 2 * padding,
        titleSize.y + descSize.y + perfTextSize.y + controlTextSize.y + 3 * lineSpacing + 2 * padding
    };
    Rectangle bgRect = rectWithSizePosDef(bgMinSize, pos);
    Vector2 textPos = Vector2AddValue(rectPos(bgRect), padding);

    DrawRectangleRec(bgRect, bgColor);
    DrawTextEx(font, item->display, textPos, fontSz, letterSpacing, normalColor);
    textPos.y += titleSize.y + lineSpacing;
    DrawTextEx(font, item->setting.description, textPos, fontSz, letterSpacing, Fade(normalColor, 0.75));
    textPos.y += descSize.y + lineSpacing;
    DrawTextEx(font, perfImpLabel, textPos, fontSz, lineSpacing, perfImpPresets[perfImp].color);
    textPos.y += perfTextSize.y + lineSpacing;
    DrawTextEx(font, controlText, textPos, fontSz, letterSpacing, normalColor);

    return bgRect;
}

Rectangle guiTextWithBG(Font font, const char* string, Sides pos, float padding, float fontSz, Color bgColor, Color fgColor) {
    float spacing = fontSz * REL_LETTER_SPACING;
    Vector2 bgMinSize = Vector2AddValue(MeasureTextEx(font, string, fontSz, spacing), padding * 2);

    Rectangle bgRect = rectWithSizePosDef(bgMinSize, pos);
    Vector2 txtPos = Vector2AddValue(rectPos(bgRect), padding);

    DrawRectangleRec(bgRect, bgColor);
    DrawTextEx(font, string, (txtPos), fontSz, spacing, fgColor);

    return bgRect;
}

Rectangle guiMinimap(Player* player, Map* map, Sides pos, float padding, float unitSize, Color bgColor, Color fgColor) {
    float mapAspectRatio = map->size.x / map->size.y;
    Vector2 mapSize = { 0 };

    if (mapAspectRatio >= 1) {
        mapSize.y = unitSize;
        mapSize.x = unitSize * mapAspectRatio;
    }
    else {
        mapSize.x = unitSize;
        mapSize.y = unitSize / mapAspectRatio;
    }

    Vector2 bgMinSize = Vector2AddValue(mapSize, padding * 2);
    Rectangle bgRect = rectWithSizePosDef(bgMinSize, pos);

    Vector2 mapPos = Vector2AddValue(rectPos(bgRect), padding);
    Vector2 playerPos = (Vector2) {
        mapPos.x + Wrap(player->pos.x / map->size.x, 0, 1) * mapSize.x,
        mapPos.y + Wrap(player->pos.y / map->size.y, 0, 1) * mapSize.y
    };

    Vector2 playerDirLine = (Vector2) {
        playerPos.x + cosf(player->hRotRad) * 20,
        playerPos.y - sinf(player->hRotRad) * 20
    };

    DrawRectangleRec(bgRect, bgColor);
    DrawRectangleLines((int)mapPos.x, (int)mapPos.y, (int)mapSize.x, (int)mapSize.y, fgColor);
    DrawLineV(playerPos, playerDirLine, fgColor);
    DrawCircleV(playerPos, 3, fgColor);

    return bgRect;
}
