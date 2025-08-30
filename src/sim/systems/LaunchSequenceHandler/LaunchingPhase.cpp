#include "LaunchingPhase.h"
#include <sstream>
#include <iomanip>

bool LaunchingPhase::isLaunchingComplete(float launchingTimer) {
    const float LAUNCHING_DURATION = 1.0f; // 1 second
    return launchingTimer >= LAUNCHING_DURATION;
}

std::string LaunchingPhase::getLaunchingMessage(float launchingTimer) {
    const float LAUNCHING_DURATION = 1.0f;
    int progress = static_cast<int>((launchingTimer / LAUNCHING_DURATION) * 100);
    
    std::stringstream ss;
    ss << "Payload State: Launching... " << progress << "%";
    return ss.str();
}
