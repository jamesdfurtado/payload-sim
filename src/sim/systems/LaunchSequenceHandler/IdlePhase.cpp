#include "IdlePhase.h"
#include "../../SimulationState.h"
#include <random>
#include <sstream>
#include <iomanip>

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

std::string IdlePhase::createCode() {
    // Generate a random 4-digit code
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    
    int code = dis(gen);
    
    // Format as 4-digit string with leading zeros if needed
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << code;
    return oss.str();
}
