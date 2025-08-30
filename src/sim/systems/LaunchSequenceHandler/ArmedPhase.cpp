#include "ArmedPhase.h"
#include "LaunchSequenceHandler.h"
#include "../../SimulationState.h"
#include <iostream>

// Create a static surveillance instance for armed phase
static PhaseSurveillance armedSurveillance = []() {
    PhaseSurveillance surveillance;
    
    // Add ALL conditions that must be met to stay in armed state
    // This includes everything from authorized phase (payload system is a state, not a condition)
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
    
    // Note: payloadSystemOperational is NOT monitored here because it's a state indicator
    // that gets set to true when entering Armed state and false when leaving
    
    return surveillance;
}();

CheckAuthorizationStatus ArmedPhase::canStayArmed(const SimulationState& state) {
    // Debug logging to see actual values of surveillance conditions
    std::cout << "[ArmedPhase] Debug - targetValidated: " << state.targetValidated 
              << ", targetAcquired: " << state.targetAcquired 
              << ", depthClearanceMet: " << state.depthClearanceMet
              << ", launchTubeIntegrity: " << state.launchTubeIntegrity
              << ", powerSupplyStable: " << state.powerSupplyStable
              << ", noFriendlyUnitsInBlastRadius: " << state.noFriendlyUnitsInBlastRadius
              << ", launchConditionsFavorable: " << state.launchConditionsFavorable
              << ", payloadSystemOperational: " << state.payloadSystemOperational << std::endl;
    
    // Use the modular surveillance system to check surveillance conditions
    return armedSurveillance.checkConditions(state);
}
