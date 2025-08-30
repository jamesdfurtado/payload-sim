#include <raylib.h>
#include <memory>
#include "sim/SimulationEngine.h"
#include "sim/systems/PowerSystem.h"
#include "sim/systems/DepthSystem.h"
#include "sim/systems/SonarSystem.h"
#include "sim/systems/TargetingSystem.h"
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "sim/systems/EnvironmentSystem.h"
#include "sim/systems/TargetAcquisitionSystem.h"
#include "sim/systems/TargetValidationSystem.h"
#include "sim/systems/FriendlySafetySystem.h"

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
    auto launchSequence = std::make_shared<LaunchSequenceHandler>(engine);
    auto crosshairManager = std::make_shared<CrosshairManager>(*contacts);
    auto targetAcquisition = std::make_shared<TargetAcquisitionSystem>(*crosshairManager, *contacts);
    auto targetValidation = std::make_shared<TargetValidationSystem>(*crosshairManager, *contacts);
    auto friendlySafety = std::make_shared<FriendlySafetySystem>(*crosshairManager, *contacts);


    engine.registerSystem(power);
    engine.registerSystem(depth);
    engine.registerSystem(sonar);
    engine.registerSystem(targeting);
    engine.registerSystem(environment);
    engine.registerSystem(launchSequence);
    engine.registerSystem(targetAcquisition);
    engine.registerSystem(targetValidation);
    engine.registerSystem(friendlySafety);

    // UI root with direct pointers to systems/state
    UIRoot ui(engine, sonar.get(), power.get(), depth.get(), targeting.get(), launchSequence.get(), environment.get(), contacts.get(), crosshairManager.get());

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


