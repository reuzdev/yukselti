#include <raylib.h>
#include "app.h"

#define WIN_W 800
#define WIN_H 600

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIN_W, WIN_H, "Yukselti");
    SetExitKey(KEY_NULL);

    App* a = appCreate();
    if (!a) {
        goto cleanup;
    }

    while (!WindowShouldClose()) {
        appUpdate(a);
        BeginDrawing();
            appDraw(a);
        EndDrawing();
    }

    appDestroy(a);
    cleanup:
    CloseWindow();
}
