#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class LaunchingPhase {
public:
    // Check if launching is complete and we can transition to Launched
    static bool isLaunchingComplete(float launchingTimer);
    
    // Get the launching progress message
    static std::string getLaunchingMessage(float launchingTimer);
};
