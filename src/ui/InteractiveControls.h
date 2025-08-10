#pragma once

#include <raylib.h>
#include <memory>
#include "InputHandler.h"
#include "AuthCode.h"

class SafetySystem;
class DepthControl;
class SimulationEngine;

class InteractiveControls {
public:
    InteractiveControls(SimulationEngine& engine, SafetySystem* safety, DepthControl* depth);

    struct UIState {
        bool depthThrottleDragging = false;
        float depthThrottleValue = 0.5f; // 0.0 = down, 0.5 = neutral, 1.0 = up
        Vector2 depthThrottleSliderPos = {0, 0};
        
        // Safety button states - track if they should be lit up
        bool authButtonLit = false;
        bool armButtonLit = false;
        bool launchButtonLit = false;
        bool resetButtonLit = false;
    };

    void update(float dt, UIState& uiState, AuthCode::AuthState& authState);
    void drawInteractiveControls(Rectangle r, const UIState& uiState, const AuthCode::AuthState& authState);
    void drawDepthControlInPanel(Rectangle depthPanelRect, const UIState& uiState);

private:
    SimulationEngine& engine;
    SafetySystem* safety;
    DepthControl* depth;
    std::unique_ptr<AuthCode> authCode;
    
    // UI Helper methods
    void updateDepthThrottle(UIState& uiState, Rectangle throttleRect);
    void updateSafetyButtons(UIState& uiState, Rectangle r, AuthCode::AuthState& authState);
    void drawDepthThrottle(Rectangle r, const UIState& uiState);
    void drawSafetyStateButtons(Rectangle r, const UIState& uiState);
    
    // Button interaction helpers
    bool isButtonPressed(Rectangle buttonRect);
    void drawLitButton(Rectangle buttonRect, const char* text, bool isLit, bool isActive, Color baseColor);
    
    // Safety button functionality
    void processArmRequest();
    void processLaunchRequest();
    void processResetRequest();
};
