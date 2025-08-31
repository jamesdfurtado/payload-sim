#include "AuthorizedPhase.h"
#include "LaunchSequenceHandler.h"
#include "../../SimulationState.h"
#include <iostream>

// Survey for all flags to be met to remain authorized
static PhaseSurveillance authorizedSurveillance = []() {
    PhaseSurveillance surveillance;
    
    // Conditions to be met
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
    // Use the surveillance
    return authorizedSurveillance.checkConditions(state);
}