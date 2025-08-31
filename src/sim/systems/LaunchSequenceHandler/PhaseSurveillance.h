#pragma once

#include "../../SimulationState.h"
#include <vector>
#include <string>
#include <functional>

struct CheckAuthorizationStatus {
    bool isAuthorized;
    std::string message;
    
    CheckAuthorizationStatus(bool isAuth, const std::string& msg) 
        : isAuthorized(isAuth), message(msg) {}
};

struct SurveillanceCondition {
    std::function<bool(const SimulationState&)> checkFunc;
    std::string failureMessage;
    
    SurveillanceCondition(std::function<bool(const SimulationState&)> func, 
                         const std::string& failMsg)
        : checkFunc(func), failureMessage(failMsg) {}
};

class PhaseSurveillance {
public:
    // Allow for adjustable surveillance conditions
    void addCondition(std::function<bool(const SimulationState&)> checkFunc, 
                     const std::string& failureMessage);
    
    // Check all conditions and return status
    CheckAuthorizationStatus checkConditions(const SimulationState& state) const;

    void clearConditions();
    size_t getConditionCount() const { return conditions.size(); }

private:
    std::vector<SurveillanceCondition> conditions;
};
