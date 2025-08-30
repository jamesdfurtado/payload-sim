#include "ResettingPhase.h"
#include <sstream>
#include <iomanip>

bool ResettingPhase::isResetComplete(float resetTimer) {
    const float RESET_DURATION = 2.0f; // 2 seconds
    return resetTimer >= RESET_DURATION;
}

std::string ResettingPhase::getResetMessage(float resetTimer) {
    const float RESET_DURATION = 2.0f;
    int progress = static_cast<int>((resetTimer / RESET_DURATION) * 100);
    
    std::stringstream ss;
    ss << "Payload State: Resetting... " << progress << "%";
    return ss.str();
}
