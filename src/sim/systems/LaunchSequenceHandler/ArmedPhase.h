#pragma once

#include "CurrentLaunchPhase.h"
#include "PhaseSurveillance.h"
#include "../../SimulationState.h"
#include <string>

class ArmedPhase {
public:
    static CheckAuthorizationStatus canStayArmed(const SimulationState& state);
};
