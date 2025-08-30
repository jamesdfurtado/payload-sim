#include "ArmingPhase.h"
#include <sstream>
#include <iomanip>

bool ArmingPhase::isArmingComplete(float armingTimer) {
    const float ARMING_DURATION = 2.0f; // 2 seconds
    return armingTimer >= ARMING_DURATION;
}

std::string ArmingPhase::getArmingMessage(float armingTimer) {
    const float ARMING_DURATION = 2.0f;
    int progress = static_cast<int>((armingTimer / ARMING_DURATION) * 100);
    
    std::stringstream ss;
    ss << "Payload State: Arming... " << progress << "%";
    return ss.str();
}
