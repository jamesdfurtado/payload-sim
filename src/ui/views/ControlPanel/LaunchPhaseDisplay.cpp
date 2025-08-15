#include "LaunchPhaseDisplay.h"

LaunchPhaseDisplay::LaunchPhaseDisplay() 
    : currentPhaseText(DEFAULT_PHASE) {
    // Set initial bounds
    setBounds(Rectangle{0, 0, 200, 50});
}

void LaunchPhaseDisplay::draw() const {
    // Draw the phase text with a prominent background box
    const char* phaseText = "Payload Phase: ";
    
    // Calculate text dimensions for the background box
    int textWidth = MeasureText(phaseText, 22);
    int phaseWidth = MeasureText(currentPhaseText, 22);
    int totalWidth = textWidth + phaseWidth + 20; // Add padding
    
    // Draw prominent background box
    Rectangle textBox = {
        bounds.x + 5,           // X position
        bounds.y - 25,          // Y position (adjusted for box height)
        (float)totalWidth,      // Width to fit both texts
        35                       // Height with padding
    };
    
    // Draw bright blue background box with border
    DrawRectangleRec(textBox, {0, 100, 200, 255});        // Bright blue background
    DrawRectangleLinesEx(textBox, 3, {255, 255, 255});    // White border
    
    // Draw the phase text
    DrawText(phaseText, bounds.x + 10, bounds.y - 15, 22, RAYWHITE);
    
    // Draw the current phase in bright yellow
    DrawText(currentPhaseText, bounds.x + 10 + textWidth, bounds.y - 15, 22, {255, 255, 0, 255});
}

void LaunchPhaseDisplay::setCurrentPhase(const char* phaseText) {
    currentPhaseText = phaseText ? phaseText : DEFAULT_PHASE;
}
