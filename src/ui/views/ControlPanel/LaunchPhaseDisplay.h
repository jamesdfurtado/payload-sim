#pragma once

#include "../../Widget.h"
#include <raylib.h>

class LaunchPhaseDisplay : public Widget {
public:
    LaunchPhaseDisplay();
    
    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override { return false; }
    bool onMouseUp(Vector2 mousePos) override { return false; }
    bool onMouseMove(Vector2 mousePos) override { return false; }
    
    // Set the current phase text
    void setCurrentPhase(const char* phaseText);
    
    // Get the current phase text
    const char* getCurrentPhase() const { return currentPhaseText; }

private:
    const char* currentPhaseText;
    static constexpr const char* DEFAULT_PHASE = "Unknown";
};
