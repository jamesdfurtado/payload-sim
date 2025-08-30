#pragma once

#include "CurrentLaunchPhase.h"
#include "PhaseSurveillance.h"
#include "../../SimulationState.h"
#include <string>

class ArmedPhase {
public:
    // Check if we can stay in Armed state (includes ALL surveillance conditions)
    static CheckAuthorizationStatus canStayArmed(const SimulationState& state);
};
