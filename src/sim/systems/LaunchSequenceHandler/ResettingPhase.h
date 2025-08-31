#pragma once

#include "CurrentLaunchPhase.h"
#include "../../SimulationState.h"
#include <string>

class ResettingPhase {
public:
    static bool isResetComplete(float resetTimer);
    static std::string getResetMessage(float resetTimer);
};
