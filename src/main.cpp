#include <raylib.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Payload Sim");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("payload-sim init", 20, 20, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


