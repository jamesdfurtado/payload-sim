#include "AuthCode.h"
#include "../systems/SafetySystem.h"
#include "../simulation/SimulationEngine.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

AuthCode::AuthCode(SimulationEngine& engine, SafetySystem* safety)
    : engine(engine), safety(safety) {
    std::srand((unsigned int)std::time(nullptr));
}

void AuthCode::update(float dt, AuthState& authState) {
    // Check if auth was requested via the AUTH button
    if (authState.authRequested) {
        processAuthRequest(authState);
        authState.authRequested = false; // Reset the flag
    }
    
    // Clear "Authorization granted" message when payload is armed or launched
    if (safety && (safety->getPhase() == LaunchPhase::Arming || 
                   safety->getPhase() == LaunchPhase::Armed || 
                   safety->getPhase() == LaunchPhase::Launching || 
                   safety->getPhase() == LaunchPhase::Launched)) {
        if (authState.feedbackMessage == "Authorization granted, please ARM the payload.") {
            authState.feedbackMessage.clear();
        }
    }
    
    // Clear auth state when reset happens
    if (safety && safety->getPhase() == LaunchPhase::Resetting) {
        clearAuthState(authState);
    }
    
    // Handle calculator number pad input
    Rectangle baseRect = { 640, 380, 620, 320 };
    Rectangle inputRect = { baseRect.x + 20, baseRect.y + 180, 120, 28 };
    Rectangle calcArea = { inputRect.x + 140, inputRect.y, 180, 120 };
    
    // Number buttons (3x3 grid + 0 at bottom)
    int buttonSize = 35;
    int spacing = 5;
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int number = (2 - row) * 3 + col + 1; // 1-9 in proper order
            Rectangle buttonRect = {
                calcArea.x + col * (buttonSize + spacing),
                calcArea.y + row * (buttonSize + spacing),
                (float)buttonSize, (float)buttonSize
            };
            
            if (isButtonPressed(buttonRect)) {
                addDigit(authState, number);
            }
        }
    }
    
    // Zero button (bottom center)
    Rectangle zeroButton = {
        calcArea.x + 1 * (buttonSize + spacing),
        calcArea.y + 3 * (buttonSize + spacing),
        (float)buttonSize, (float)buttonSize
    };
    if (isButtonPressed(zeroButton)) {
        addDigit(authState, 0);
    }
    
    // Delete button (bottom right)
    Rectangle deleteButton = {
        calcArea.x + 2 * (buttonSize + spacing),
        calcArea.y + 3 * (buttonSize + spacing),
        (float)buttonSize, (float)buttonSize
    };
    if (isButtonPressed(deleteButton)) {
        deleteLastDigit(authState);
    }
    

}

void AuthCode::drawAuthCode(Rectangle r, const AuthState& authState) {
    // Draw compact auth input box (4 digits only)
    Rectangle authBox = { r.x + 20, r.y + 180, 120, 28 };
    DrawText("Enter code:", (int)authBox.x, (int)authBox.y - 22, 18, SKYBLUE);
    DrawRectangleLinesEx(authBox, 2, authState.inputCode.length() == 4 ? GREEN : GRAY);
    
    // Draw input with fixed-width spacing for 4 digits
    std::string displayCode = authState.inputCode;
    while (displayCode.length() < 4) displayCode += "_";
    
    for (int i = 0; i < 4; i++) {
        char digit[2] = { displayCode[i], '\0' };
        DrawText(digit, (int)authBox.x + 8 + i * 25, (int)authBox.y + 4, 18, SKYBLUE);
    }
    
    // Draw calculator number pad
    Rectangle calcArea = { authBox.x + 140, authBox.y, 180, 120 };
    
    int buttonSize = 35;
    int spacing = 5;
    
    // Number buttons (1-9)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int number = (2 - row) * 3 + col + 1; // 1-9 in proper order
            Rectangle buttonRect = {
                calcArea.x + col * (buttonSize + spacing),
                calcArea.y + row * (buttonSize + spacing),
                (float)buttonSize, (float)buttonSize
            };
            
            drawNumberButton(buttonRect, number, false);
        }
    }
    
    // Zero button
    Rectangle zeroButton = {
        calcArea.x + 1 * (buttonSize + spacing),
        calcArea.y + 3 * (buttonSize + spacing),
        (float)buttonSize, (float)buttonSize
    };
    drawNumberButton(zeroButton, 0, false);
    
    // Delete button
    Rectangle deleteButton = {
        calcArea.x + 2 * (buttonSize + spacing),
        calcArea.y + 3 * (buttonSize + spacing),
        (float)buttonSize, (float)buttonSize
    };
    drawDeleteButton(deleteButton, false);
    
    // AUTH challenge code display (always visible, populated when active)
    Rectangle challengeArea = { authBox.x, authBox.y + 40, 300, 60 };
    DrawText("AUTH CODE:", (int)challengeArea.x, (int)challengeArea.y, 16, SKYBLUE);
    
    if (!authState.challengeCode.empty()) {
        // Draw challenge code box
        Rectangle codeBox = { challengeArea.x, challengeArea.y + 20, 120, 35 };
        DrawRectangleRec(codeBox, Fade(BLACK, 0.8f));
        DrawRectangleLinesEx(codeBox, 2, SKYBLUE);
        DrawText(authState.challengeCode.c_str(), (int)codeBox.x + 25, (int)codeBox.y + 8, 24, YELLOW);
    } else {
        // Draw placeholder
        Rectangle codeBox = { challengeArea.x, challengeArea.y + 20, 120, 35 };
        DrawRectangleLinesEx(codeBox, 1, DARKGRAY);
        DrawText("----", (int)codeBox.x + 35, (int)codeBox.y + 8, 24, DARKGRAY);
    }
    
    // Show feedback messages
    if (!authState.feedbackMessage.empty()) {
        DrawText(authState.feedbackMessage.c_str(), (int)r.x + 20, (int)r.y + 130, 16, LIGHTGRAY);
    }
    

}

void AuthCode::drawNumberButton(Rectangle buttonRect, int number, bool isPressed) {
    Color buttonColor = isPressed ? LIGHTGRAY : DARKGRAY;
    Color textColor = RAYWHITE;
    
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
        buttonColor = ColorBrightness(buttonColor, 0.2f);
    }
    
    DrawRectangleRec(buttonRect, buttonColor);
    DrawRectangleLinesEx(buttonRect, 1, GRAY);
    
    char numStr[2] = { static_cast<char>('0' + number), '\0' };
    int textWidth = MeasureText(numStr, 16);
    DrawText(numStr, 
             (int)(buttonRect.x + (buttonRect.width - textWidth) / 2),
             (int)(buttonRect.y + (buttonRect.height - 16) / 2),
             16, textColor);
}

void AuthCode::drawDeleteButton(Rectangle buttonRect, bool isPressed) {
    Color buttonColor = isPressed ? LIGHTGRAY : MAROON;
    Color textColor = RAYWHITE;
    
    if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
        buttonColor = ColorBrightness(buttonColor, 0.2f);
    }
    
    DrawRectangleRec(buttonRect, buttonColor);
    DrawRectangleLinesEx(buttonRect, 1, GRAY);
    
    // Draw back arrow symbol
    DrawText("<-", (int)(buttonRect.x + 8), (int)(buttonRect.y + (buttonRect.height - 16) / 2), 16, textColor);
}

bool AuthCode::isButtonPressed(Rectangle buttonRect) {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), buttonRect);
}

bool AuthCode::checkAllConditionsExceptAuth() {
    auto& s = engine.getState();
    return s.targetValidated && s.targetAcquired && s.depthClearanceMet && 
           s.launchTubeIntegrity && s.powerSupplyStable && 
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

void AuthCode::processAuthRequest(AuthState& authState) {
    if (checkAllConditionsExceptAuth()) {
        // Generate random 4-digit code
        int code = 1000 + std::rand() % 9000;
        authState.challengeCode = std::to_string(code);
        if (safety) safety->setChallengeCode(authState.challengeCode);
        authState.inputCode.clear();
        authState.feedbackMessage = "Enter the AUTH code shown below.";
    } else {
        authState.feedbackMessage = "Cannot request AUTH: All conditions (except Authorization) must be met.";
    }
}

void AuthCode::addDigit(AuthState& authState, int digit) {
    if (authState.inputCode.length() < 4) {
        authState.inputCode += ('0' + digit);
        
        // Auto-submit when 4 digits are entered
        if (authState.inputCode.length() == 4 && !authState.challengeCode.empty()) {
            if (authState.inputCode == authState.challengeCode) {
                if (checkAllConditionsExceptAuth()) {
                    if (safety) safety->requestAuthorizationCode(authState.inputCode.c_str());
                    authState.feedbackMessage = "Authorization granted, please ARM the payload.";
                } else {
                    authState.feedbackMessage = "Cannot authorize: All conditions must be met.";
                }
            } else {
                authState.feedbackMessage = "Incorrect AUTH code.";
            }
        }
    }
}

void AuthCode::deleteLastDigit(AuthState& authState) {
    if (!authState.inputCode.empty()) {
        authState.inputCode.pop_back();
    }
}

void AuthCode::requestAuthCode(AuthState& authState) {
    // This method is called when the AUTH button is pressed
    // It will trigger the auth request process
    authState.authRequested = true;
}

void AuthCode::clearAuthState(AuthState& authState) {
    // Clear all auth-related data
    authState.inputCode.clear();
    authState.challengeCode.clear();
    authState.feedbackMessage.clear();
    authState.authRequested = false;
}
