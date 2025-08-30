#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class ResettingPhase {
public:
    // Check if reset is complete and we can transition to Idle
    static bool isResetComplete(float resetTimer);
    
    // Get the reset progress message
    static std::string getResetMessage(float resetTimer);
};
