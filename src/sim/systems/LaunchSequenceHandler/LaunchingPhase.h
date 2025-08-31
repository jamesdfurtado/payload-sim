#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class LaunchingPhase {
public:
    static bool isLaunchingComplete(float launchingTimer);
    static std::string getLaunchingMessage(float launchingTimer);
};
