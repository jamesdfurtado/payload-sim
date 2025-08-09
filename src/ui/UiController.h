#pragma once

#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/PowerSystem.h"
#include "../systems/DepthControl.h"
#include "../systems/TargetingSystem.h"
#include "../systems/SafetySystem.h"
#include "../systems/EnvironmentSystem.h"

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

    SonarSystem* sonar = nullptr;
    PowerSystem* power = nullptr;
    DepthControl* depth = nullptr;
    TargetingSystem* targeting = nullptr;
    SafetySystem* safety = nullptr;
    EnvironmentSystem* environment = nullptr;

    // UI state
    float uiWeaponsPower = 0.5f;
    std::string feedbackMessage;
    std::string authCodeInput;
    bool awaitingAuthCode = false;
    bool authBoxFocused = false;
    std::string authChallengeCode;
    float resetTimer = 0.0f;
    bool localAuthGranted = false;

    // Missile animation state
    bool missileActive = false;
    Vector2 missilePos{0,0};
    Vector2 missileVel{0,0};
    Vector2 missileTarget{0,0};
    std::vector<Vector2> missileTrail;
    float missileProgress = 0.0f;
    float missileExplosionTimer = 0.0f;
    int missileTargetIdx = -1;

    // Helpers
    void drawStatusLights(const SimulationState& s, Rectangle r);
    void drawPower(Rectangle r);
    void drawDepth(Rectangle r);
    void drawSonar(Rectangle r);
    void drawControls(Rectangle r);

    Vector2 worldToScreen(Vector2 p, Rectangle r);
    Vector2 screenToWorld(Vector2 p, Rectangle r);

    static constexpr float MISSILE_SPEED = 200.0f;
    static constexpr float EXPLOSION_DURATION = 1.0f;
};


