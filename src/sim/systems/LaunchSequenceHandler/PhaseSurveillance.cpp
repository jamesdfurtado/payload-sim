#include "PhaseSurveillance.h"
#include <iostream>

void PhaseSurveillance::addCondition(std::function<bool(const SimulationState&)> checkFunc, 
                                     const std::string& failureMessage) {
    conditions.emplace_back(checkFunc, failureMessage);
}

CheckAuthorizationStatus PhaseSurveillance::checkConditions(const SimulationState& state) const {
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

void PhaseSurveillance::clearConditions() {
    conditions.clear();
}
