#pragma once

#include "CurrentLaunchPhase.h"
#include <string>
#include "../../SimulationState.h"

struct AuthorizationResult {
    bool canAuthorize;
    std::string message;
    
    AuthorizationResult(bool canAuth, const std::string& msg) 
        : canAuthorize(canAuth), message(msg) {}
};

class IdlePhase {
public:
    // Check if we can move from Idle to Authorized state
    static AuthorizationResult canAuthorize(const SimulationState& state);
    
    // Generate a random 4-digit authorization code
    static std::string createCode();
    
private:
    // Helper methods to check individual conditions
    static bool checkTargetValidated(const SimulationState& state);
    static bool checkTargetAcquired(const SimulationState& state);
    static bool checkDepthClearanceMet(const SimulationState& state);
    static bool checkLaunchTubeIntegrity(const SimulationState& state);
    static bool checkPowerSupplyStable(const SimulationState& state);
    static bool checkNoFriendlyUnitsInBlastRadius(const SimulationState& state);
    static bool checkLaunchConditionsFavorable(const SimulationState& state);
};
