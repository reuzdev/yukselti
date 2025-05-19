#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include "player.h"
#include "common.h"
#include "map.h"

void playerControl(Player* player, Map* map) {
    playerMove2D(player, map);
    playerMoveUpDown(player, map);
    playerRotateH(player);
    playerRotateV(player);
}

void playerMove2D(Player* player, Map* map) {
    player->moveSpeed = calcNextSpeed(
        player->moveSpeed,
        player->maxMoveSpeed,
        player->accel,
        player->decel,
        KEY_GO_FORWARD,
        KEY_GO_BACKWARD
    );

    float xPos = player->pos.x + player->moveSpeed * cosf(player->hRotRad) * GetFrameTime();
    float yPos = player->pos.y - player->moveSpeed * sinf(player->hRotRad) * GetFrameTime();
    player->pos.x = Wrap(xPos, 0, map->size.x);
    player->pos.y = Wrap(yPos, 0, map->size.y);
}

void playerMoveUpDown(Player* player, Map* map) {
    player->ascentSpeed = calcNextSpeed(
        player->ascentSpeed,
        player->maxAscentSpeed,
        player->accel,
        player->decel,
        KEY_FLY_UP,
        KEY_FLY_DOWN
    );

    float newAltitude = player->altitude + player->ascentSpeed * GetFrameTime();
    player->altitude = MAX(newAltitude, mapGetCell(map, player->pos, 0).height);
}

void playerRotateH(Player* player) {
    player->hRotSpeedRad = calcNextSpeed(
        player->hRotSpeedRad,
        player->maxHRotSpeedRad,
        player->hRotAccelRad,
        player->hRotAccelRad,
        KEY_TURN_LEFT,
        KEY_TURN_RIGHT
    );

    player->hRotRad += player->hRotSpeedRad * GetFrameTime();
}

void playerRotateV(Player* player) {
    player->vRotSpeedRad = calcNextSpeed(
        player->vRotSpeedRad,
        player->maxVRotSpeedRad,
        player->vRotAccelRad,
        player->vRotAccelRad,
        KEY_LOOK_UP,
        KEY_LOOK_DOWN
    );

    player->vRotRad += player->vRotSpeedRad * GetFrameTime();
    if (player->vRotRad > VROT_HALF_RANGE || player->vRotRad < -VROT_HALF_RANGE) {
        player->vRotRad = Clamp(player->vRotRad, -VROT_HALF_RANGE, VROT_HALF_RANGE);
        player->vRotSpeedRad = 0;
    }
}

float calcNextSpeed(float speed, float absMax, float accel, float decel, KeyboardKey accelKey, KeyboardKey decelKey) {
    int inputDir = IsKeyDown(accelKey) - IsKeyDown(decelKey);
    if (inputDir) {
        float delta = inputDir * accel * GetFrameTime();
        return Clamp(speed + delta, -absMax, absMax);
    }
    else {
        float absDelta = decel * GetFrameTime();
        if (fabsf(speed) < absDelta) {
            return 0;
        }
        else {
            return speed - (speed > 0 ? 1 : -1) * absDelta;
        }
    }
}
