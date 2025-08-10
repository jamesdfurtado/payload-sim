#include "UiController.h"

UiController::UiController(SimulationEngine& engine,
                           SonarSystem* sonar,
                           PowerSystem* power,
                           DepthControl* depth,
                           TargetingSystem* targeting,
                           SafetySystem* safety,
                           EnvironmentSystem* environment)
    : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety), environment(environment) {
    
    // Initialize UI components
    inputHandler = std::make_unique<InputHandler>(engine, sonar, power, depth, targeting, safety);
    statusPanel = std::make_unique<StatusPanel>(engine, power);
    controlsPanel = std::make_unique<ControlsPanel>(safety);
    sonarDisplay = std::make_unique<SonarDisplay>(engine, sonar, safety);
}

void UiController::update(float dt) {
    // Delegate input handling to InputHandler
    inputHandler->handleInput(dt, inputState);
    
    // Handle sonar clicks
    Rectangle sonarRect{ 20, 120, 600, 580 };
    inputHandler->handleSonarClick(sonarRect);
    
    // Update missile animation
    sonarDisplay->updateMissileAnimation(dt, missileState, sonarRect);
}

void UiController::render() {
    Rectangle status = { 640, 20, 620, 110 };
    Rectangle powerR = { 640, 140, 620, 110 };
    Rectangle depthR = { 640, 260, 620, 110 };
    Rectangle controls = { 640, 380, 620, 320 };
    Rectangle sonarR = { 20, 120, 600, 580 };

    DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE);

    // Delegate rendering to UI components
    statusPanel->drawStatusLights(status);
    statusPanel->drawPower(powerR, inputState.uiWeaponsPower);
    statusPanel->drawDepth(depthR);
    controlsPanel->drawControls(controls, inputState);
    sonarDisplay->drawSonar(sonarR, missileState);
}




