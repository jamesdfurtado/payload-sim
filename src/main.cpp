#include <raylib.h>
#include <memory>
#include "sim/SimulationEngine.h"
#include "sim/systems/PowerSystem.h"
#include "sim/systems/DepthSystem.h"
#include "sim/systems/SonarSystem.h"
#include "sim/systems/TargetingSystem.h"
#include "sim/systems/SafetySystem.h"
#include "sim/systems/EnvironmentSystem.h"
#include "sim/systems/TargetAcquisitionSystem.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"
#include "ui/UIRoot.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Submarine Payload Launch (New)");
    SetTargetFPS(60);

    // Simulation core
    SimulationEngine engine;
    auto contacts = std::make_shared<ContactManager>();
    auto power = std::make_shared<PowerSystem>();
    auto depth = std::make_shared<DepthSystem>();
    auto sonar = std::make_shared<SonarSystem>(*contacts);
    auto targeting = std::make_shared<TargetingSystem>();
    auto environment = std::make_shared<EnvironmentSystem>();
    auto safety = std::make_shared<SafetySystem>();
    auto crosshairManager = std::make_shared<CrosshairManager>(*contacts);
    auto targetAcquisition = std::make_shared<TargetAcquisitionSystem>(*crosshairManager, *contacts);

    engine.registerSystem(power);
    engine.registerSystem(depth);
    engine.registerSystem(sonar);
    engine.registerSystem(targeting);
    engine.registerSystem(environment);
    engine.registerSystem(safety);
    engine.registerSystem(targetAcquisition);


    // these need to be moved out of main.cpp at some point.
    // Set a simple challenge code to allow authorization via the new UI
    safety->setChallengeCode("1234");
    // Ensure power turns off after safety reset completes
    safety->setPowerOffCallback([power]{ power->setPowerLevel(0.0f); });

    // UI root with direct pointers to systems/state
    UIRoot ui(engine, sonar.get(), power.get(), depth.get(), targeting.get(), safety.get(), environment.get(), contacts.get(), crosshairManager.get());

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        engine.update(dt);
        ui.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        ui.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


