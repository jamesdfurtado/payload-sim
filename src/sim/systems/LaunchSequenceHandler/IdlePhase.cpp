#include "IdlePhase.h"
#include "../../SimulationState.h"

AuthorizationResult IdlePhase::canAuthorize(const SimulationState& state) {
    // Define all conditions and their failure messages
    struct Condition {
        bool (*checkFunc)(const SimulationState&);
        const char* failureMessage;
    };
    
    static const Condition conditions[] = {
        {checkTargetValidated, "Target not validated. "},
        {checkTargetAcquired, "Target not acquired. "},
        {checkDepthClearanceMet, "Depth clearance not met. "},
        {checkLaunchTubeIntegrity, "Launch tube integrity compromised. "},
        {checkPowerSupplyStable, "Power supply unstable. "},
        {checkNoFriendlyUnitsInBlastRadius, "Friendly units in blast radius. "},
        {checkLaunchConditionsFavorable, "Launch conditions unfavorable. "}
    };
    
    // Check all conditions
    std::string message;
    bool canAuth = true;
    
    for (const auto& condition : conditions) {
        if (!condition.checkFunc(state)) {
            message += condition.failureMessage;
            canAuth = false;
        }
    }
    
    if (canAuth) {
        message = "All authorization conditions met. Ready to authorize.";
    }
    
    return AuthorizationResult(canAuth, message);
}

bool IdlePhase::checkTargetValidated(const SimulationState& state) {
    return state.targetValidated;
}

bool IdlePhase::checkTargetAcquired(const SimulationState& state) {
    return state.targetAcquired;
}

bool IdlePhase::checkDepthClearanceMet(const SimulationState& state) {
    return state.depthClearanceMet;
}

bool IdlePhase::checkLaunchTubeIntegrity(const SimulationState& state) {
    return state.launchTubeIntegrity;
}

bool IdlePhase::checkPowerSupplyStable(const SimulationState& state) {
    return state.powerSupplyStable;
}

bool IdlePhase::checkNoFriendlyUnitsInBlastRadius(const SimulationState& state) {
    return state.noFriendlyUnitsInBlastRadius;
}

bool IdlePhase::checkLaunchConditionsFavorable(const SimulationState& state) {
    return state.launchConditionsFavorable;
}
