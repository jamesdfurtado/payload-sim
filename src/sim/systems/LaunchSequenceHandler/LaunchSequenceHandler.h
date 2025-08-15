#pragma once

#include "CurrentLaunchPhase.h"
#include "IdlePhase.h"

class SimulationEngine; // forward declaration

class LaunchSequenceHandler {
public:
    explicit LaunchSequenceHandler(SimulationEngine& engine);
    ~LaunchSequenceHandler();

    // Request functions for UI button presses
    void requestAuthorization();
    void submitAuthorization(const std::string& inputCode);
    void requestArm();
    void requestLaunch();
    void requestReset();

    // Get current phase
    CurrentLaunchPhase getCurrentPhase() const;
    
    // Get current phase as string for display
    const char* getCurrentPhaseString() const;
    
    // Get the current authorization code for UI display
    const std::string& getAuthCode() const;
    
    // Check if authorization is pending (waiting for code input)
    bool isAuthorizationPending() const;
    
    // Force clear the authorization code (useful for external reset)
    void clearAuthCode();
    
private:
    CurrentLaunchPhase currentPhase;
    SimulationEngine& engine;
    std::string authCode; // Stores the generated 4-digit authorization code
};
