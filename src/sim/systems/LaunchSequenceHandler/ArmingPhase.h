#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class ArmingPhase {
public:
    // Check if arming is complete and we can transition to Armed
    static bool isArmingComplete(float armingTimer);
    
    // Get the arming progress message
    static std::string getArmingMessage(float armingTimer);
};
