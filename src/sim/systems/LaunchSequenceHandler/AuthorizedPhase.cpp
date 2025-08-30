#include "AuthorizedPhase.h"
#include "LaunchSequenceHandler.h"
#include "../../SimulationState.h"
#include <iostream>

CheckAuthorizationStatus AuthorizedPhase::canStayAuthorized(const SimulationState& state) {
    // Define all conditions and their failure messages (excluding checkPayloadSystemOperational as requested)
    struct Condition {
        bool (*checkFunc)(const SimulationState&);
        const char* failureMessage;
    };
    
    static const Condition conditions[] = {
        {LaunchSequenceHandler::checkTargetValidated, "Target no longer validated. "},
        {LaunchSequenceHandler::checkTargetAcquired, "Target no longer acquired. "},
        {LaunchSequenceHandler::checkDepthClearanceMet, "Depth clearance no longer met. "},
        {LaunchSequenceHandler::checkLaunchTubeIntegrity, "Launch tube integrity compromised. "},
        {LaunchSequenceHandler::checkPowerSupplyStable, "Power supply unstable. "},
        {LaunchSequenceHandler::checkNoFriendlyUnitsInBlastRadius, "Friendly units in blast radius. "},
        {LaunchSequenceHandler::checkLaunchConditionsFavorable, "Launch conditions unfavorable. "}
    };
    
    // Debug logging to see actual values
    std::cout << "[AuthorizedPhase] Debug - targetValidated: " << state.targetValidated 
              << ", targetAcquired: " << state.targetAcquired 
              << ", depthClearanceMet: " << state.depthClearanceMet
              << ", launchTubeIntegrity: " << state.launchTubeIntegrity
              << ", powerSupplyStable: " << state.powerSupplyStable
              << ", noFriendlyUnitsInBlastRadius: " << state.noFriendlyUnitsInBlastRadius
              << ", launchConditionsFavorable: " << state.launchConditionsFavorable << std::endl;
    
    // Check all conditions
    std::string message;
    bool isAuth = true;
    
    for (const auto& condition : conditions) {
        if (!condition.checkFunc(state)) {
            message += condition.failureMessage;
            isAuth = false;
        }
    }
    
    if (isAuth) {
        message = "All authorization maintenance conditions met. Can stay authorized.";
    }
    
    return CheckAuthorizationStatus(isAuth, message);
}
