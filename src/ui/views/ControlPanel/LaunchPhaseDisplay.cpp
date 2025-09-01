#include "LaunchPhaseDisplay.h"

LaunchPhaseDisplay::LaunchPhaseDisplay() 
    : currentPhaseText(DEFAULT_PHASE) {

    setBounds(Rectangle{0, 0, 200, 50});
}

void LaunchPhaseDisplay::draw() const {

    const char* phaseText = "Payload Phase: ";
    
    int textWidth = MeasureText(phaseText, 22);
    int phaseWidth = MeasureText(currentPhaseText, 22);
    int totalWidth = textWidth + phaseWidth + 20; // with padding
    
    Rectangle textBox = {
        bounds.x + 5,
        bounds.y - 25,
        (float)totalWidth,
        35
    };
    
    DrawRectangleRec(textBox, {0, 100, 200, 255});
    DrawRectangleLinesEx(textBox, 3, {255, 255, 255});
    
    DrawText(phaseText, bounds.x + 10, bounds.y - 15, 22, RAYWHITE);
    DrawText(currentPhaseText, bounds.x + 10 + textWidth, bounds.y - 15, 22, {255, 255, 0, 255});
}

void LaunchPhaseDisplay::setCurrentPhase(const char* phaseText) {
    currentPhaseText = phaseText ? phaseText : DEFAULT_PHASE;
}
