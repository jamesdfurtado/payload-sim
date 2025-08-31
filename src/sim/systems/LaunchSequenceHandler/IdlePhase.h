#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

// Forward declaration
class LaunchSequenceHandler;

struct AuthorizationResult {
    bool canAuthorize;
    std::string message;
    
    AuthorizationResult(bool canAuth, const std::string& msg) 
        : canAuthorize(canAuth), message(msg) {}
};

class IdlePhase {
public:
    static AuthorizationResult canAuthorize(const SimulationState& state);
    static std::string createCode();
};