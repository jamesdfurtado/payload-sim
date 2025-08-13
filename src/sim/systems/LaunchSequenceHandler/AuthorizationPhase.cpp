#include "AuthorizationPhase.h"
#include <iostream>

AuthorizationResult AuthorizationPhase::validateRequirements(const SimulationState& state) {
    AuthorizationResult result;
    result.success = true;
    result.failedRequirements.clear();
    
    // Check each requirement and collect failures
    if (!checkTargetValidated(state)) {
        result.success = false;
        result.failedRequirements.push_back("Target not validated");
    }
    
    if (!checkTargetAcquired(state)) {
        result.success = false;
        result.failedRequirements.push_back("Target not acquired");
    }
    
    if (!checkDepthClearanceMet(state)) {
        result.success = false;
        result.failedRequirements.push_back("Depth clearance not met");
    }
    
    if (!checkLaunchTubeIntegrity(state)) {
        result.success = false;
        result.failedRequirements.push_back("Launch tube integrity compromised");
    }
    
    if (!checkPowerSupplyStable(state)) {
        result.success = false;
        result.failedRequirements.push_back("Power supply not stable");
    }
    
    if (!checkNoFriendlyUnitsInBlastRadius(state)) {
        result.success = false;
        result.failedRequirements.push_back("Friendly units in blast radius");
    }
    
    if (!checkLaunchConditionsFavorable(state)) {
        result.success = false;
        result.failedRequirements.push_back("Launch conditions not favorable");
    }
    
    // Set result message
    if (result.success) {
        result.message = "SUCCESS!! All launch requirements met.";
        std::cout << "AUTHORIZATION: SUCCESS!! All launch requirements met." << std::endl;
    } else {
        result.message = "FAILED!! Launch requirements not met:";
        std::cout << "AUTHORIZATION: FAILED!! Launch requirements not met:" << std::endl;
        for (const auto& failure : result.failedRequirements) {
            std::cout << "  - " << failure << std::endl;
        }
    }
    
    return result;
}

bool AuthorizationPhase::checkTargetValidated(const SimulationState& state) const {
    return state.targetValidated;
}

bool AuthorizationPhase::checkTargetAcquired(const SimulationState& state) const {
    return state.targetAcquired;
}

bool AuthorizationPhase::checkDepthClearanceMet(const SimulationState& state) const {
    return state.depthClearanceMet;
}

bool AuthorizationPhase::checkLaunchTubeIntegrity(const SimulationState& state) const {
    return state.launchTubeIntegrity;
}

bool AuthorizationPhase::checkPowerSupplyStable(const SimulationState& state) const {
    return state.powerSupplyStable;
}

bool AuthorizationPhase::checkNoFriendlyUnitsInBlastRadius(const SimulationState& state) const {
    return state.noFriendlyUnitsInBlastRadius;
}

bool AuthorizationPhase::checkLaunchConditionsFavorable(const SimulationState& state) const {
    return state.launchConditionsFavorable;
}
