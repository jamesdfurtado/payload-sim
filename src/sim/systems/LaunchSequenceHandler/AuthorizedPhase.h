#pragma once

#include "CurrentLaunchPhase.h"
#include "PhaseSurveillance.h"
#include "../../SimulationState.h"
#include <string>

// Forward declaration
class LaunchSequenceHandler;

class AuthorizedPhase {
public:
    static CheckAuthorizationStatus canStayAuthorized(const SimulationState& state);
};