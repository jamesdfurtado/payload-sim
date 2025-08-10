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
    
    // Show current launch phase
    if (safety) {
        const char* phaseStr = getPhaseString(inputState);
        DrawText(TextFormat("Payload State: %s", phaseStr), (int)r.x + 10, (int)r.y + 170, 20, YELLOW);
    }
    
    // Show phase-specific messages
    drawPhaseSpecificMessage(r, inputState);
    
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

const char* ControlsPanel::getPhaseString(const InputHandler::InputState& inputState) {
    if (!safety) return "UNKNOWN";
    
    switch (safety->getPhase()) {
        case LaunchPhase::Idle: return "IDLE";
        case LaunchPhase::Authorized: return "AUTHORIZED";
        case LaunchPhase::Arming: return "ARMING";
        case LaunchPhase::Armed: return "ARMED";
        case LaunchPhase::Launching: return "LAUNCHING";
        case LaunchPhase::Launched: return "LAUNCHED";
        case LaunchPhase::Resetting: return (inputState.resetTimer > 0.0f ? "RESETTING..." : "RESET");
        default: return "UNKNOWN";
    }
}

void ControlsPanel::drawPhaseSpecificMessage(Rectangle r, const InputHandler::InputState& inputState) {
    if (safety) {
        switch (safety->getPhase()) {
            case LaunchPhase::Arming:
                DrawText("Arming payload...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Launching:
                DrawText("Launching payload...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Launched:
                DrawText("Payload LAUNCHED!", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Resetting:
                if (!safety->getResetReason().empty()) {
                    DrawText(TextFormat("System resetting: %s", safety->getResetReason().c_str()), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                } else {
                    DrawText("System RESETTING...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                }
                break;
            default:
                if (!inputState.feedbackMessage.empty()) {
                    DrawText(inputState.feedbackMessage.c_str(), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                }
                break;
        }
    } else if (!inputState.feedbackMessage.empty()) {
        DrawText(inputState.feedbackMessage.c_str(), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
    }
}
