#include <ui.h>
#include <raylib.h>

#define FACTOR 100
#define WIDTH 16 * FACTOR
#define HEIGHT 9 * FACTOR

int main() {

    InitWindow(WIDTH, HEIGHT, "Entity Data Genearater");
    Image windowIcon = LoadImage("res/icon.png");
    SetWindowIcon(windowIcon);
    SetTargetFPS(60);
    ui_init();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818ff));

        ui_update();

        EndDrawing();
    }
    ui_unload();
    CloseWindow();
    return 0;
}
