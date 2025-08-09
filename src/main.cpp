#include <raylib.h>
#include "simulation/SimulationEngine.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Payload Sim");
    SetTargetFPS(60);

    SimulationEngine engine;

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        engine.update(dt);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("payload-sim init", 20, 20, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


