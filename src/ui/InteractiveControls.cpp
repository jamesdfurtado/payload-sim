#include "InteractiveControls.h"
#include "../systems/SafetySystem.h"
#include "../systems/DepthControl.h"
#include "../simulation/SimulationEngine.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

InteractiveControls::InteractiveControls(SimulationEngine& engine, SafetySystem* safety, DepthControl* depth)
    : engine(engine), safety(safety), depth(depth) {
    std::srand((unsigned int)std::time(nullptr));
}

void InteractiveControls::update(float dt, UIState& uiState, InputHandler::InputState& inputState) {
    // Update safety button lighting based on current state
    if (safety) {
        LaunchPhase phase = safety->getPhase();
        uiState.authButtonLit = (phase != LaunchPhase::Idle && phase != LaunchPhase::Resetting);
        uiState.armButtonLit = (phase == LaunchPhase::Arming || phase == LaunchPhase::Armed || 
                               phase == LaunchPhase::Launching || phase == LaunchPhase::Launched);
        uiState.launchButtonLit = (phase == LaunchPhase::Launching || phase == LaunchPhase::Launched);
        uiState.resetButtonLit = (phase == LaunchPhase::Resetting);
    }
    
    // Handle reset timer countdown
    if (inputState.resetTimer > 0.0f && safety && safety->getPhase() == LaunchPhase::Resetting) {
        inputState.resetTimer -= dt;
        if (inputState.resetTimer <= 0.0f) {
            if (safety) safety->forceIdle();
            inputState.feedbackMessage = "System ready.";
        }
        return;
    }
    
    // Handle auth input if focused - preserve existing auth functionality
    if (inputState.authBoxFocused) {
        // Keep existing auth input handling from InputHandler
        return;
    }
    
    // Calculate UI rectangles for interactive elements
    Rectangle controlsRect = { 640, 380, 620, 320 };
    Rectangle depthPanelRect = { 640, 260, 620, 110 };
    Rectangle throttleRect = { depthPanelRect.x + 200, depthPanelRect.y + 35, 50, depthPanelRect.height - 45 };
    
    // Update depth throttle
    updateDepthThrottle(uiState, throttleRect);
    
    // Update safety buttons
    updateSafetyButtons(uiState, inputState, controlsRect);
    
    // Apply depth throttle to depth control system
    if (depth) {
        depth->setBallastThrottle(uiState.depthThrottleValue);
    }
}

void InteractiveControls::updateDepthThrottle(UIState& uiState, Rectangle throttleRect) {
    Vector2 mousePos = GetMousePosition();
    
    // Check if mouse is pressed and over the throttle area
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, throttleRect)) {
            uiState.depthThrottleDragging = true;
        }
    }
    
    // Handle dragging
    if (uiState.depthThrottleDragging) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            // Calculate throttle value based on mouse Y position
            float relativeY = (mousePos.y - throttleRect.y) / throttleRect.height;
            relativeY = std::max(0.0f, std::min(1.0f, relativeY));
            // Invert Y so that top = 1.0 (up) and bottom = 0.0 (down)
            uiState.depthThrottleValue = 1.0f - relativeY;
        } else {
            uiState.depthThrottleDragging = false;
        }
    }
    
    // Update slider position for rendering
    float sliderY = throttleRect.y + (1.0f - uiState.depthThrottleValue) * throttleRect.height;
    uiState.depthThrottleSliderPos = { throttleRect.x + throttleRect.width / 2, sliderY };
}

void InteractiveControls::updateSafetyButtons(UIState& uiState, InputHandler::InputState& inputState, Rectangle r) {
    // Suppress unused parameter warning
    (void)uiState;
    // Define button positions - moved to left side with better spacing
    Rectangle authButton = { r.x + 20, r.y + 50, 110, 40 };
    Rectangle armButton = { r.x + 140, r.y + 50, 110, 40 };
    Rectangle launchButton = { r.x + 260, r.y + 50, 110, 40 };
    Rectangle resetButton = { r.x + 380, r.y + 50, 110, 40 };
    
    // Handle button clicks
    if (isButtonPressed(authButton)) {
        processAuthRequest(inputState);
    }
    if (isButtonPressed(armButton)) {
        processArmRequest(inputState);
    }
    if (isButtonPressed(launchButton)) {
        processLaunchRequest(inputState);
    }
    if (isButtonPressed(resetButton)) {
        processResetRequest(inputState);
    }
}

void InteractiveControls::drawInteractiveControls(Rectangle r, const UIState& uiState, const InputHandler::InputState& inputState) {
    DrawRectangleRec(r, Fade(DARKBROWN, 0.3f));
    DrawText("Interactive Controls", (int)r.x + 10, (int)r.y + 8, 20, ORANGE);
    
    // Draw safety state buttons
    drawSafetyStateButtons(r, uiState, inputState);
    
    // Show current launch phase - positioned below buttons
    if (safety) {
        const char* phaseStr = "";
        switch (safety->getPhase()) {
            case LaunchPhase::Idle: phaseStr = "IDLE"; break;
            case LaunchPhase::Authorized: phaseStr = "AUTHORIZED"; break;
            case LaunchPhase::Arming: phaseStr = "ARMING"; break;
            case LaunchPhase::Armed: phaseStr = "ARMED"; break;
            case LaunchPhase::Launching: phaseStr = "LAUNCHING"; break;
            case LaunchPhase::Launched: phaseStr = "LAUNCHED"; break;
            case LaunchPhase::Resetting: phaseStr = (inputState.resetTimer > 0.0f ? "RESETTING..." : "RESET"); break;
            default: phaseStr = "UNKNOWN"; break;
        }
        DrawText(TextFormat("Payload State: %s", phaseStr), (int)r.x + 20, (int)r.y + 110, 18, YELLOW);
    }
    
    // Show feedback messages - positioned below payload state
    if (safety) {
        switch (safety->getPhase()) {
            case LaunchPhase::Arming:
                DrawText("Arming payload...", (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                break;
            case LaunchPhase::Launching:
                DrawText("Launching payload...", (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                break;
            case LaunchPhase::Launched:
                DrawText("Payload LAUNCHED!", (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                break;
            case LaunchPhase::Resetting:
                if (!safety->getResetReason().empty()) {
                    DrawText(TextFormat("System resetting: %s", safety->getResetReason().c_str()), (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                } else {
                    DrawText("System RESETTING...", (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                }
                break;
            default:
                if (!inputState.feedbackMessage.empty()) {
                    DrawText(inputState.feedbackMessage.c_str(), (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
                }
                break;
        }
    } else if (!inputState.feedbackMessage.empty()) {
        DrawText(inputState.feedbackMessage.c_str(), (int)r.x + 20, (int)r.y + 140, 16, LIGHTGRAY);
    }
    
    // Draw auth input area - preserve existing functionality - moved to bottom
    Rectangle authBox = { r.x + 20, r.y + 230, 300, 28 };
    DrawText("AUTH CODE:", (int)authBox.x, (int)authBox.y - 22, 18, SKYBLUE);
    DrawRectangleLinesEx(authBox, 2, inputState.authBoxFocused ? YELLOW : GRAY);
    DrawText(inputState.authCodeInput.c_str(), (int)authBox.x + 8, (int)authBox.y + 4, 18, SKYBLUE);
    
    if (inputState.authBoxFocused) {
        DrawRectangle((int)authBox.x + 8 + MeasureText(inputState.authCodeInput.c_str(), 18), (int)authBox.y + 6, 12, 18, SKYBLUE);
    }
    
    // Show AUTH challenge code
    if (inputState.authBoxFocused && !inputState.authChallengeCode.empty()) {
        int boxW = 220, boxH = 60;
        int boxX = (int)authBox.x + 320;
        int boxY = (int)authBox.y - 10;
        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.85f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, SKYBLUE);
        DrawText("AUTH CHALLENGE", boxX + 16, boxY + 8, 18, SKYBLUE);
        DrawText(inputState.authChallengeCode.c_str(), boxX + 60, boxY + 32, 28, YELLOW);
    }
}

void InteractiveControls::drawDepthThrottle(Rectangle r, const UIState& uiState) {
    Rectangle throttleRect = { r.x + 10, r.y + 80, 60, 180 };
    
    // Draw title
    DrawText("DEPTH", (int)throttleRect.x, (int)throttleRect.y - 50, 16, LIME);
    DrawText("CONTROL", (int)throttleRect.x - 5, (int)throttleRect.y - 30, 16, LIME);
    
    // Draw throttle background
    DrawRectangleRec(throttleRect, Fade(DARKGRAY, 0.8f));
    DrawRectangleLinesEx(throttleRect, 3, LIME);
    
    // Draw center line (neutral position)
    float centerY = throttleRect.y + throttleRect.height / 2;
    DrawLine((int)throttleRect.x, (int)centerY, (int)(throttleRect.x + throttleRect.width), (int)centerY, YELLOW);
    
    // Draw throttle track
    Rectangle trackRect = { throttleRect.x + throttleRect.width / 2 - 2, throttleRect.y + 5, 4, throttleRect.height - 10 };
    DrawRectangleRec(trackRect, DARKGRAY);
    
    // Draw throttle handle
    float handleSize = 15;
    Vector2 handlePos = uiState.depthThrottleSliderPos;
    DrawCircleV(handlePos, handleSize, uiState.depthThrottleDragging ? ORANGE : RAYWHITE);
    DrawCircleLinesV(handlePos, handleSize, BLACK);
    
    // Draw labels
    DrawText("UP", (int)throttleRect.x + 5, (int)throttleRect.y - 20, 14, LIGHTGRAY);
    DrawText("NEUTRAL", (int)throttleRect.x + 80, (int)centerY - 7, 14, YELLOW);
    DrawText("DOWN", (int)throttleRect.x, (int)(throttleRect.y + throttleRect.height + 5), 14, LIGHTGRAY);
    
    // Draw current depth control value - positioned to avoid overlap
    const char* direction = "";
    if (uiState.depthThrottleValue > 0.55f) direction = "ASCENDING";
    else if (uiState.depthThrottleValue < 0.45f) direction = "DESCENDING";
    else direction = "HOLDING";
    
    DrawText(TextFormat("Depth: %s", direction), (int)throttleRect.x + 80, (int)throttleRect.y + 10, 16, RAYWHITE);
    DrawText(TextFormat("Throttle: %.0f%%", (uiState.depthThrottleValue - 0.5f) * 200.0f), (int)throttleRect.x + 80, (int)throttleRect.y + 30, 14, LIGHTGRAY);
}

void InteractiveControls::drawSafetyStateButtons(Rectangle r, const UIState& uiState, const InputHandler::InputState& inputState) {
    // Suppress unused parameter warning
    (void)inputState;
    // Define button positions - moved to left side with better spacing
    Rectangle authButton = { r.x + 20, r.y + 50, 110, 40 };
    Rectangle armButton = { r.x + 140, r.y + 50, 110, 40 };
    Rectangle launchButton = { r.x + 260, r.y + 50, 110, 40 };
    Rectangle resetButton = { r.x + 380, r.y + 50, 110, 40 };
    
    // Draw safety sequence progression label
    DrawText("Safety Sequence:", (int)r.x + 20, (int)r.y + 30, 16, LIGHTGRAY);
    
    // Determine if buttons should be active (clickable) based on current state
    bool authActive = true; // Always can try to auth
    bool armActive = safety && safety->getPhase() == LaunchPhase::Authorized;
    bool launchActive = safety && safety->getPhase() == LaunchPhase::Armed;
    bool resetActive = safety && safety->getPhase() != LaunchPhase::Idle && safety->getPhase() != LaunchPhase::Resetting;
    
    // Draw buttons with appropriate lighting and states
    drawLitButton(authButton, "AUTH", uiState.authButtonLit, authActive, DARKBLUE);
    drawLitButton(armButton, "ARM", uiState.armButtonLit, armActive, DARKPURPLE);
    drawLitButton(launchButton, "LAUNCH", uiState.launchButtonLit, launchActive, DARKGREEN);
    drawLitButton(resetButton, "RESET", uiState.resetButtonLit, resetActive, MAROON);
}

bool InteractiveControls::isButtonPressed(Rectangle buttonRect) {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), buttonRect);
}

void InteractiveControls::drawLitButton(Rectangle buttonRect, const char* text, bool isLit, bool isActive, Color baseColor) {
    Color buttonColor = baseColor;
    Color textColor = LIGHTGRAY;
    Color borderColor = GRAY;
    
    if (isLit) {
        // Button is lit up - make it bright and glowing
        buttonColor = ColorBrightness(baseColor, 0.4f);
        textColor = RAYWHITE;
        borderColor = RAYWHITE;
    }
    
    if (!isActive) {
        // Button is disabled - make it darker
        buttonColor = ColorBrightness(buttonColor, -0.5f);
        textColor = DARKGRAY;
        borderColor = DARKGRAY;
    }
    
    // Draw button with hover effect
    if (isActive && CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
        buttonColor = ColorBrightness(buttonColor, 0.2f);
        borderColor = RAYWHITE;
    }
    
    DrawRectangleRec(buttonRect, buttonColor);
    DrawRectangleLinesEx(buttonRect, 2, borderColor);
    
    // Center the text
    int textWidth = MeasureText(text, 16);
    int textX = (int)(buttonRect.x + (buttonRect.width - textWidth) / 2);
    int textY = (int)(buttonRect.y + (buttonRect.height - 16) / 2);
    DrawText(text, textX, textY, 16, textColor);
    
    // Add glow effect for lit buttons
    if (isLit && isActive) {
        DrawRectangleLinesEx(buttonRect, 1, Fade(RAYWHITE, 0.3f));
    }
}

// Auth-related methods - preserve existing functionality from InputHandler
bool InteractiveControls::checkAllConditionsExceptAuth() {
    auto& s = engine.getState();
    return s.targetValidated && s.targetAcquired && s.depthClearanceMet && 
           s.launchTubeIntegrity && s.powerSupplyStable && 
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

void InteractiveControls::processAuthRequest(InputHandler::InputState& inputState) {
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
    }
}

void InteractiveControls::processArmRequest(InputHandler::InputState& inputState) {
    if (safety) {
        if (safety->getPhase() != LaunchPhase::Authorized) {
            inputState.feedbackMessage = "Cannot ARM: Not authorized.";
        } else {
            safety->arm();
            inputState.feedbackMessage.clear();
        }
    }
}

void InteractiveControls::processLaunchRequest(InputHandler::InputState& inputState) {
    if (safety) {
        if (safety->getPhase() != LaunchPhase::Armed) {
            inputState.feedbackMessage = "Cannot LAUNCH: Not ARMED.";
        } else {
            safety->launch();
        }
    }
}

void InteractiveControls::processResetRequest(InputHandler::InputState& inputState) {
    if (safety) {
        safety->reset();
        inputState.feedbackMessage = "System RESETTING...";
        inputState.resetTimer = 2.0f;
    }
}

void InteractiveControls::drawDepthControlInPanel(Rectangle depthPanelRect, const UIState& uiState) {
    // Create throttle rect that's better positioned within the depth panel
    Rectangle throttleRect = { depthPanelRect.x + 200, depthPanelRect.y + 35, 50, depthPanelRect.height - 45 };
    
    // Draw throttle background
    DrawRectangleRec(throttleRect, Fade(DARKGRAY, 0.8f));
    DrawRectangleLinesEx(throttleRect, 2, LIME);
    
    // Draw center line (neutral position)
    float centerY = throttleRect.y + throttleRect.height / 2;
    DrawLine((int)throttleRect.x, (int)centerY, (int)(throttleRect.x + throttleRect.width), (int)centerY, YELLOW);
    
    // Draw throttle track
    Rectangle trackRect = { throttleRect.x + throttleRect.width / 2 - 2, throttleRect.y + 5, 4, throttleRect.height - 10 };
    DrawRectangleRec(trackRect, DARKGRAY);
    
    // Draw throttle handle
    float handleSize = 12;
    Vector2 handlePos = uiState.depthThrottleSliderPos;
    DrawCircleV(handlePos, handleSize, uiState.depthThrottleDragging ? ORANGE : RAYWHITE);
    DrawCircleLinesV(handlePos, handleSize, BLACK);
    
    // Draw labels spread out to use more space
    DrawText("UP", (int)throttleRect.x + 70, (int)throttleRect.y + 5, 16, LIGHTGRAY);
    DrawText("NEUTRAL", (int)throttleRect.x + 70, (int)centerY - 8, 16, YELLOW);
    DrawText("DOWN", (int)throttleRect.x + 70, (int)(throttleRect.y + throttleRect.height - 20), 16, LIGHTGRAY);
    
    // Draw current depth control value with better spacing - moved further right
    const char* direction = "";
    if (uiState.depthThrottleValue > 0.55f) direction = "ASCENDING";
    else if (uiState.depthThrottleValue < 0.45f) direction = "DESCENDING";
    else direction = "HOLDING";
    
    DrawText(direction, (int)depthPanelRect.x + 450, (int)depthPanelRect.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Throttle: %.0f%%", (uiState.depthThrottleValue - 0.5f) * 200.0f), (int)depthPanelRect.x + 450, (int)depthPanelRect.y + 65, 16, LIGHTGRAY);
}
