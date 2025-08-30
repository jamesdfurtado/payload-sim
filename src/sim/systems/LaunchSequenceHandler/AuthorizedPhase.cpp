#include "AuthorizedPhase.h"
#include "LaunchSequenceHandler.h"
#include "../../SimulationState.h"
#include <iostream>

// Create a static surveillance instance for authorized phase
static PhaseSurveillance authorizedSurveillance = []() {
    PhaseSurveillance surveillance;
    
    // Add all the conditions that must be met to stay in authorized state
    surveillance.addCondition(
        LaunchSequenceHandler::checkTargetValidated, 
        "Target no longer validated. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkTargetAcquired, 
        "Target no longer acquired. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkDepthClearanceMet, 
        "Depth clearance no longer met. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkLaunchTubeIntegrity, 
        "Launch tube integrity compromised. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkPowerSupplyStable, 
        "Power supply unstable. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkNoFriendlyUnitsInBlastRadius, 
        "Friendly units in blast radius. "
    );
    surveillance.addCondition(
        LaunchSequenceHandler::checkLaunchConditionsFavorable, 
        "Launch conditions unfavorable. "
    );
    
    return surveillance;
}();

CheckAuthorizationStatus AuthorizedPhase::canStayAuthorized(const SimulationState& state) {
    // Use the modular surveillance system to check all conditions
    // Debug logging removed to prevent spam - only log when conditions actually fail
    return authorizedSurveillance.checkConditions(state);
}
