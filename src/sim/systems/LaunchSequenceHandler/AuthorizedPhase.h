#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

// Forward declaration to avoid circular dependency
class LaunchSequenceHandler;

struct CheckAuthorizationStatus {
    bool isAuthorized;
    std::string message;
    
    CheckAuthorizationStatus(bool isAuth, const std::string& msg) 
        : isAuthorized(isAuth), message(msg) {}
};

class AuthorizedPhase {
public:
    // Check if we can stay in Authorized state (surveyor function)
    static CheckAuthorizationStatus canStayAuthorized(const SimulationState& state);
};
