#pragma once

#include <raylib.h>
#include <string>
#include "InputHandler.h"

class SafetySystem;
class SimulationEngine;

class AuthCode {
public:
    AuthCode(SimulationEngine& engine, SafetySystem* safety);

    struct AuthState {
        std::string inputCode = "";          // User's input (max 4 digits)
        std::string challengeCode = "";      // Challenge code from system
        std::string feedbackMessage = "";   // Error/success messages
        bool authBoxFocused = false;        // For compatibility
        bool authRequested = false;         // Flag to trigger auth request
        float resetTimer = 0.0f;            // Timer for reset operations
    };

    void update(float dt, AuthState& authState);
    void drawAuthCode(Rectangle r, const AuthState& authState);
    void requestAuthCode(AuthState& authState);
    void clearAuthState(AuthState& authState);

private:
    SimulationEngine& engine;
    SafetySystem* safety;
    
    // Calculator grid helpers
    void drawNumberButton(Rectangle buttonRect, int number, bool isPressed);
    void drawDeleteButton(Rectangle buttonRect, bool isPressed);
    bool isButtonPressed(Rectangle buttonRect);
    
    // Auth logic
    bool checkAllConditionsExceptAuth();
    void processAuthRequest(AuthState& authState);
    void addDigit(AuthState& authState, int digit);
    void deleteLastDigit(AuthState& authState);
};
