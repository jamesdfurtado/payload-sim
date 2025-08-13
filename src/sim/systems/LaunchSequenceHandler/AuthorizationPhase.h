#pragma once

#include "../../SimulationState.h"
#include "CommonTypes.h"
#include <string>
#include <vector>

class AuthorizationPhase {
public:
    AuthorizationPhase() = default;
    ~AuthorizationPhase() = default;
    
    // Validate if we can transition from Idle to Authorized
    AuthorizationResult validateRequirements(const SimulationState& state);
    
private:
    // Check individual requirement flags
    bool checkTargetValidated(const SimulationState& state) const;
    bool checkTargetAcquired(const SimulationState& state) const;
    bool checkDepthClearanceMet(const SimulationState& state) const;
    bool checkLaunchTubeIntegrity(const SimulationState& state) const;
    bool checkPowerSupplyStable(const SimulationState& state) const;
    bool checkNoFriendlyUnitsInBlastRadius(const SimulationState& state) const;
    bool checkLaunchConditionsFavorable(const SimulationState& state) const;
};
