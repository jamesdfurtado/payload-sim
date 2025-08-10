#pragma once

#include <raylib.h>
#include <string>
#include "InputHandler.h"

class SafetySystem;

class ControlsPanel {
public:
    ControlsPanel(SafetySystem* safety);

    void drawControls(Rectangle r, const InputHandler::InputState& inputState);
    void drawAuthInput(Rectangle r, const InputHandler::InputState& inputState);

private:
    SafetySystem* safety;
};
