#include "LaunchSequenceHandler.h"
#include <iostream>

LaunchSequenceHandler::LaunchSequenceHandler() 
    : currentPhase(CurrentLaunchPhase::Idle) {
    // Initialize with Idle phase
}

LaunchSequenceHandler::~LaunchSequenceHandler() {
    // TODO: Cleanup if needed
}

void LaunchSequenceHandler::requestAuthorization() {
    std::cout << "[LaunchSequenceHandler] Authorization requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Idle) {
        // TODO: Implement authorization validation logic
        // if that succeeds, change the phase to Authorized
        currentPhase = CurrentLaunchPhase::Authorized;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Authorized" << std::endl;
    }
}

void LaunchSequenceHandler::requestArm() {
    std::cout << "[LaunchSequenceHandler] Arm requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Authorized) {
        // TODO: Implement arm validation logic
        // if that succeeds, change the phase to Armed
        currentPhase = CurrentLaunchPhase::Armed;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Armed" << std::endl;
    }
}

void LaunchSequenceHandler::requestLaunch() {
    std::cout << "[LaunchSequenceHandler] Launch requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Armed) {
        // TODO: Implement launch validation logic
        // if that succeeds, change the phase to Launched
        currentPhase = CurrentLaunchPhase::Launched;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Launched" << std::endl;
    }
}

void LaunchSequenceHandler::requestReset() {
    std::cout << "[LaunchSequenceHandler] Reset requested" << std::endl;
    currentPhase = CurrentLaunchPhase::Idle;
    std::cout << "[LaunchSequenceHandler] Phase reset to: Idle" << std::endl;
}

CurrentLaunchPhase LaunchSequenceHandler::getCurrentPhase() const {
    return currentPhase;
}

const char* LaunchSequenceHandler::getCurrentPhaseString() const {
    return ::getCurrentPhaseString(currentPhase);
}
