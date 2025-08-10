#pragma once

#include <raylib.h>
#include <string>

class SimulationEngine;
class SonarSystem;
class PowerSystem;
class DepthControl;
class TargetingSystem;
class SafetySystem;

class InputHandler {
public:
    InputHandler(SimulationEngine& engine,
                 SonarSystem* sonar,
                 PowerSystem* power,
                 DepthControl* depth,
                 TargetingSystem* targeting,
                 SafetySystem* safety);

    struct InputState {
        bool authBoxFocused = false;
        std::string authCodeInput;
        std::string authChallengeCode;
        std::string feedbackMessage;
        float uiWeaponsPower = 0.5f;
        float resetTimer = 0.0f;
    };

    void handleInput(float dt, InputState& inputState);
    void handleAuthInput(InputState& inputState);
    void handleSonarClick(const Rectangle& sonarRect);

private:
    SimulationEngine& engine;
    SonarSystem* sonar;
    PowerSystem* power;
    DepthControl* depth;
    TargetingSystem* targeting;
    SafetySystem* safety;

    bool checkAllConditionsExceptAuth();
    void processAuthRequest(InputState& inputState);
    void processArmRequest(InputState& inputState);
    void processLaunchRequest(InputState& inputState);
    void processResetRequest(InputState& inputState);
};
