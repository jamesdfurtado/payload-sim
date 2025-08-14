#include "LaunchPhaseDisplay.h"

LaunchPhaseDisplay::LaunchPhaseDisplay() 
    : currentPhaseText(DEFAULT_PHASE) {
    // Set initial bounds
    setBounds(Rectangle{0, 0, 200, 50});
}

void LaunchPhaseDisplay::draw() const {
    // Draw the phase display background
    DrawRectangleRec(bounds, {25, 25, 25, 200});
    
    // Draw the phase text
    const char* phaseText = "Payload Phase: ";
    DrawText(phaseText, bounds.x + 10, bounds.y + 15, 16, RAYWHITE);
    
    // Draw the current phase in yellow
    DrawText(currentPhaseText, bounds.x + 10 + MeasureText(phaseText, 16), bounds.y + 15, 16, YELLOW);
}

void LaunchPhaseDisplay::setCurrentPhase(const char* phaseText) {
    currentPhaseText = phaseText ? phaseText : DEFAULT_PHASE;
}
