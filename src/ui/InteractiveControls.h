#pragma once

#include <raylib.h>
#include "InputHandler.h"

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

    void update(float dt, UIState& uiState, InputHandler::InputState& inputState);
    void drawInteractiveControls(Rectangle r, const UIState& uiState, const InputHandler::InputState& inputState);
    void drawDepthControlInPanel(Rectangle depthPanelRect, const UIState& uiState);

private:
    SimulationEngine& engine;
    SafetySystem* safety;
    DepthControl* depth;
    
    // UI Helper methods
    void updateDepthThrottle(UIState& uiState, Rectangle throttleRect);
    void updateSafetyButtons(UIState& uiState, InputHandler::InputState& inputState, Rectangle r);
    void drawDepthThrottle(Rectangle r, const UIState& uiState);
    void drawSafetyStateButtons(Rectangle r, const UIState& uiState, const InputHandler::InputState& inputState);
    
    // Button interaction helpers
    bool isButtonPressed(Rectangle buttonRect);
    void drawLitButton(Rectangle buttonRect, const char* text, bool isLit, bool isActive, Color baseColor);
    
    // Auth functionality that needs to be preserved
    bool checkAllConditionsExceptAuth();
    void processAuthRequest(InputHandler::InputState& inputState);
    void processArmRequest(InputHandler::InputState& inputState);
    void processLaunchRequest(InputHandler::InputState& inputState);
    void processResetRequest(InputHandler::InputState& inputState);
};
