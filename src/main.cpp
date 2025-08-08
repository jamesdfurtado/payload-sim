#include <raylib.h>
#include "simulation/SimulationEngine.h"
#include "ui/UiController.h"
#include "systems/PowerSystem.h"
#include "systems/DepthControl.h"
#include "systems/EnvironmentSystem.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Payload Sim");
    SetTargetFPS(60);

    SimulationEngine engine;

    UiController ui(engine);

    auto power = std::make_shared<PowerSystem>();
    auto depth = std::make_shared<DepthControl>();
    auto env   = std::make_shared<EnvironmentSystem>();
    engine.registerSystem(power);
    engine.registerSystem(depth);
    engine.registerSystem(env);

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        engine.update(dt);
        ui.update(dt);
        BeginDrawing();
        ClearBackground(BLACK);
        ui.render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


