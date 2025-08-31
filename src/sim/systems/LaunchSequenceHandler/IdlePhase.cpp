#include "IdlePhase.h"
#include "LaunchSequenceHandler.h"
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
        {LaunchSequenceHandler::checkTargetValidated, "Target not validated. "},
        {LaunchSequenceHandler::checkTargetAcquired, "Target not acquired. "},
        {LaunchSequenceHandler::checkDepthClearanceMet, "Depth clearance not met. "},
        {LaunchSequenceHandler::checkLaunchTubeIntegrity, "Launch tube integrity compromised. "},
        {LaunchSequenceHandler::checkPowerSupplyStable, "Power supply unstable. "},
        {LaunchSequenceHandler::checkNoFriendlyUnitsInBlastRadius, "Friendly units in blast radius. "},
        {LaunchSequenceHandler::checkLaunchConditionsFavorable, "Launch conditions unfavorable. "}
    };
    
    // Check if conditions are met to authorize
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

// Generate authorization code
std::string IdlePhase::createCode() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    
    int code = dis(gen);
    
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << code;
    return oss.str();
}
