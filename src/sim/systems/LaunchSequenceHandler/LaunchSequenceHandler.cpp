#include "LaunchSequenceHandler.h"
#include "../../SimulationEngine.h"
#include <iostream>

LaunchSequenceHandler::LaunchSequenceHandler(SimulationEngine& engine) 
    : currentPhase(CurrentLaunchPhase::Idle), engine(engine) { // Initialize with Idle phase
}

LaunchSequenceHandler::~LaunchSequenceHandler() {
    // TODO: Cleanup if needed
}

void LaunchSequenceHandler::requestAuthorization() {
    std::cout << "[LaunchSequenceHandler] Authorization requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Idle) {
        // Check if we can authorize using IdlePhase validation with real state
        const auto& state = engine.getState();
        AuthorizationResult result = IdlePhase::canAuthorize(state);
        
        if (result.canAuthorize) {
            currentPhase = CurrentLaunchPhase::Authorized;
            // Set the canLaunchAuthorized flag to true in the simulation state
            engine.getState().canLaunchAuthorized = true;
            std::cout << "[LaunchSequenceHandler] Phase changed to: Authorized" << std::endl;
            std::cout << "[LaunchSequenceHandler] " << result.message << std::endl;
        } else {
            std::cout << "[LaunchSequenceHandler] Authorization denied: " << result.message << std::endl;
        }
    } else {
        std::cout << "[LaunchSequenceHandler] Cannot authorize from current phase: " 
                  << ::getCurrentPhaseString(currentPhase) << std::endl;
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
    // Reset the canLaunchAuthorized flag to false when returning to Idle
    engine.getState().canLaunchAuthorized = false;
    std::cout << "[LaunchSequenceHandler] Phase reset to: Idle" << std::endl;
}

CurrentLaunchPhase LaunchSequenceHandler::getCurrentPhase() const {
    return currentPhase;
}

const char* LaunchSequenceHandler::getCurrentPhaseString() const {
    return ::getCurrentPhaseString(currentPhase);
}
