#include <memory>
#include <raylib.h>
#include "ui/UiController.h"
#include "simulation/SimulationEngine.h"
#include "systems/SonarSystem.h"
#include "systems/PowerSystem.h"
#include "systems/DepthControl.h"
#include "systems/TargetingSystem.h"
#include "systems/SafetySystem.h"
#include "systems/EnvironmentSystem.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Submarine Payload Launch Simulator");
    SetTargetFPS(60);

    SimulationEngine engine;

    auto power = std::make_shared<PowerSystem>();
    auto depth = std::make_shared<DepthControl>();
    auto sonar = std::make_shared<SonarSystem>();
    auto targeting = std::make_shared<TargetingSystem>();
    auto environment = std::make_shared<EnvironmentSystem>();
    auto safety = std::make_shared<SafetySystem>();

    engine.registerSystem(power);
    engine.registerSystem(depth);
    engine.registerSystem(sonar);
    engine.registerSystem(targeting);
    engine.registerSystem(environment);
    engine.registerSystem(safety);

    UiController ui(engine, sonar.get(), power.get(), depth.get(), targeting.get(), safety.get(), environment.get());

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
