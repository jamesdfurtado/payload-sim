#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

// Forward declaration to avoid circular dependency
class LaunchSequenceHandler;

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
};
