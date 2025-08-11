#pragma once

#include <memory>
#include <raylib.h>
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/PowerSystem.h"
#include "../systems/DepthControl.h"
#include "../systems/TargetingSystem.h"
#include "../systems/SafetySystem.h"
#include "../systems/EnvironmentSystem.h"
#include "../systems/ContactManager.h"
#include "InputHandler.h"
#include "StatusPanel.h"
#include "SonarDisplay.h"
#include "InteractiveControls.h"
#include "AuthCode.h"
#include "LoggingSystem.h"

class UiController {
public:
    UiController(SimulationEngine& engine,
                 SonarSystem* sonar,
                 PowerSystem* power,
                 DepthControl* depth,
                 TargetingSystem* targeting,
                 SafetySystem* safety,
                 EnvironmentSystem* environment);

    void update(float dt);
    void render();

private:
    SimulationEngine& engine;
    
    // Component dependencies
    SonarSystem* sonar = nullptr;
    PowerSystem* power = nullptr;
    DepthControl* depth = nullptr;
    TargetingSystem* targeting = nullptr;
    SafetySystem* safety = nullptr;
    EnvironmentSystem* environment = nullptr;

    // UI Components
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<SonarDisplay> sonarDisplay;
    std::unique_ptr<InteractiveControls> interactiveControls;
    std::unique_ptr<LoggingSystem> loggingSystem;

    // Component state
    InputHandler::InputState inputState;
    MissileState missileState;  // Now using MissileState from ContactManager
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    float uiPowerLevel = 0.0f; // Power level state (starts OFF)
};


