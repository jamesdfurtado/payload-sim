#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class ArmingPhase {
public:
    static bool isArmingComplete(float armingTimer);
    static std::string getArmingMessage(float armingTimer);
};
