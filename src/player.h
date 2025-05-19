#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

typedef struct Map Map;

typedef struct Player {
    Vector2 pos;
    float altitude;
    float maxMoveSpeed;
    float moveSpeed;
    float maxAscentSpeed;
    float ascentSpeed;
    float accel;
    float decel;
    float hRotRad;
    float vRotRad;
    float maxHRotSpeedRad;
    float maxVRotSpeedRad;
    float hRotSpeedRad;
    float vRotSpeedRad;
    float hRotAccelRad;
    float vRotAccelRad;
} Player;

void playerControl(Player* player, Map* map);
void playerMove2D(Player* player, Map* map);
void playerMoveUpDown(Player* player, Map* map);
void playerRotateH(Player* player);
void playerRotateV(Player* player);
float calcNextSpeed(float speed, float absMax, float accel, float decel, KeyboardKey accelKey, KeyboardKey decelKey);

#endif // PLAYER_H
