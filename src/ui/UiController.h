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
#include "InputHandler.h"
#include "StatusPanel.h"
#include "ControlsPanel.h"
#include "SonarDisplay.h"
#include "InteractiveControls.h"

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
    std::unique_ptr<ControlsPanel> controlsPanel;
    std::unique_ptr<SonarDisplay> sonarDisplay;
    std::unique_ptr<InteractiveControls> interactiveControls;

    // Component state
    InputHandler::InputState inputState;
    SonarDisplay::MissileState missileState;
    InteractiveControls::UIState uiState;
};


