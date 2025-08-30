#pragma once

#include "CurrentLaunchPhase.h"
#include "PhaseSurveillance.h"
#include "../../SimulationState.h"
#include <string>

// Forward declaration to avoid circular dependency
class LaunchSequenceHandler;

class AuthorizedPhase {
public:
    // Check if we can stay in Authorized state (surveyor function)
    static CheckAuthorizationStatus canStayAuthorized(const SimulationState& state);
};
