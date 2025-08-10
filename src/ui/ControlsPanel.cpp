#include "ControlsPanel.h"
#include "../systems/SafetySystem.h"

ControlsPanel::ControlsPanel(SafetySystem* safety)
    : safety(safety) {
}

void ControlsPanel::drawControls(Rectangle r, const InputHandler::InputState& inputState) {
    DrawRectangleRec(r, Fade(DARKBROWN, 0.3f));
    DrawText("Controls", (int)r.x + 10, (int)r.y + 8, 20, ORANGE);
    DrawText("W/S ballast | A/D power | Q/E stabilize | Click sonar to lock", (int)r.x + 10, (int)r.y + 40, 18, GRAY);
    DrawText("1) Request AUTH", (int)r.x + 10, (int)r.y + 70, 18, RAYWHITE);
    DrawText("2) ARM", (int)r.x + 10, (int)r.y + 94, 18, RAYWHITE);
    DrawText("3) LAUNCH", (int)r.x + 10, (int)r.y + 118, 18, RAYWHITE);
    DrawText("4) RESET", (int)r.x + 10, (int)r.y + 142, 18, RAYWHITE);
    
    // Payload state and phase-specific messages are now handled by the centralized LoggingSystem
    
    // Draw auth input area
    drawAuthInput(r, inputState);
}

void ControlsPanel::drawAuthInput(Rectangle r, const InputHandler::InputState& inputState) {
    // Draw AUTH code label and text box
    Rectangle authBox = { r.x + 10, r.y + 280, 300, 28 };
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

// Phase string generation is now handled by the centralized LoggingSystem

// Phase-specific messages are now handled by the centralized LoggingSystem
