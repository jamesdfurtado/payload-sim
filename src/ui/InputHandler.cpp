#include "InputHandler.h"
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/PowerSystem.h"
#include "../systems/DepthControl.h"
#include "../systems/TargetingSystem.h"
#include "../systems/SafetySystem.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

InputHandler::InputHandler(SimulationEngine& engine,
                           SonarSystem* sonar,
                           PowerSystem* power,
                           DepthControl* depth,
                           TargetingSystem* targeting,
                           SafetySystem* safety)
    : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety) {
    std::srand((unsigned int)std::time(nullptr));
}

void InputHandler::handleInput(float dt, InputState& inputState) {
    // Handle RESETTING... state
    if (inputState.resetTimer > 0.0f && safety && safety->getPhase() == LaunchPhase::Resetting) {
        inputState.resetTimer -= dt;
        if (inputState.resetTimer <= 0.0f) {
            if (safety) safety->forceIdle();
            inputState.feedbackMessage = "System ready.";
        }
        return;
    }

    // Handle auth input if focused
    if (inputState.authBoxFocused) {
        handleAuthInput(inputState);
        return;
    }

    // Targeting stability controls (keep these keyboard controls)
    if (IsKeyDown(KEY_Q)) { if (targeting) targeting->adjustStability(-0.4f * dt); }
    if (IsKeyDown(KEY_E)) { if (targeting) targeting->adjustStability(+0.4f * dt); }

    // NOTE: Power controls (A/D), depth controls (W/S), and safety sequence (1-4) are now handled by UI components
}

void InputHandler::handleAuthInput(InputState& inputState) {
    Rectangle controls = { 640, 380, 620, 320 };
    Rectangle authBox = { controls.x + 20, controls.y + 230, 300, 28 };

    // Handle mouse clicks for auth box focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (!CheckCollisionPointRec(m, authBox)) {
            inputState.authBoxFocused = false;
        }
    }

    // Handle keyboard input
    if (IsKeyPressed(KEY_BACKSPACE) && !inputState.authCodeInput.empty()) {
        inputState.authCodeInput.pop_back();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        inputState.authBoxFocused = false;
        inputState.feedbackMessage = "Authorization cancelled.";
        return;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (safety) {
            if (inputState.authCodeInput == inputState.authChallengeCode) {
                if (checkAllConditionsExceptAuth()) {
                    safety->requestAuthorizationCode(inputState.authCodeInput.c_str());
                    inputState.feedbackMessage = "Authorization granted, please ARM the payload.";
                } else {
                    inputState.feedbackMessage = "Cannot authorize: All conditions (except Authorization) must be met.";
                }
            } else {
                inputState.feedbackMessage = "Incorrect AUTH code.";
            }
        }
        inputState.authBoxFocused = false;
        return;
    }

    // Handle character input
    int key = GetCharPressed();
    while (key > 0) {
        if (key < 128 && std::isprint(key)) {
            inputState.authCodeInput += (char)key;
        }
        key = GetCharPressed();
    }
}

void InputHandler::handleSonarClick(const Rectangle& sonarRect) {
    if (sonar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, sonarRect)) {
            // Convert screen to world coordinates
            float nx = (m.x - sonarRect.x) / sonarRect.width;
            float ny = (m.y - sonarRect.y) / sonarRect.height;
            Vector2 world = { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
            sonar->attemptManualLock(world);
        }
    }
}

bool InputHandler::checkAllConditionsExceptAuth() {
    auto& s = engine.getState();
    return s.targetValidated && s.targetAcquired && s.depthClearanceMet && 
           s.launchTubeIntegrity && s.powerSupplyStable && 
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

void InputHandler::processAuthRequest(InputState& inputState) {
    if (!inputState.authBoxFocused) {
        if (checkAllConditionsExceptAuth()) {
            inputState.authBoxFocused = true;
            // Generate random 4-digit code
            int code = 1000 + std::rand() % 9000;
            inputState.authChallengeCode = std::to_string(code);
            if (safety) safety->setChallengeCode(inputState.authChallengeCode);
            inputState.authCodeInput.clear();
            inputState.feedbackMessage = "Enter the AUTH code shown below.";
        } else {
            inputState.feedbackMessage = "Cannot request AUTH: All conditions (except Authorization) must be met.";
            inputState.authBoxFocused = false;
        }
    } else {
        // Submit auth code
        if (safety) {
            if (inputState.authCodeInput == inputState.authChallengeCode) {
                if (checkAllConditionsExceptAuth()) {
                    safety->requestAuthorizationCode(inputState.authCodeInput.c_str());
                    inputState.feedbackMessage = "Authorization granted, please ARM the payload.";
                } else {
                    inputState.feedbackMessage = "Cannot authorize: All conditions (except Authorization) must be met.";
                }
            } else {
                inputState.feedbackMessage = "Incorrect AUTH code.";
            }
        }
        inputState.authBoxFocused = false;
    }
}

void InputHandler::processArmRequest(InputState& inputState) {
    if (safety) {
        if (safety->getPhase() != LaunchPhase::Authorized) {
            inputState.feedbackMessage = "Cannot ARM: Not authorized.";
        } else {
            safety->arm();
            inputState.feedbackMessage.clear();
        }
    }
}

void InputHandler::processLaunchRequest(InputState& inputState) {
    if (safety) {
        if (safety->getPhase() != LaunchPhase::Armed) {
            inputState.feedbackMessage = "Cannot LAUNCH: Not ARMED.";
        } else {
            safety->launch();
        }
    }
}

void InputHandler::processResetRequest(InputState& inputState) {
    if (safety) {
        safety->reset();
        inputState.feedbackMessage = "System RESETTING...";
        inputState.resetTimer = 2.0f;
    }
}
