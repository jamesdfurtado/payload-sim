#pragma once

#include "CurrentLaunchPhase.h"
#include "IdlePhase.h"
#include "AuthorizedPhase.h"
#include "../../SimulationState.h"
#include "../../ISystem.h"
#include <string>

class SimulationEngine; // forward declaration

class LaunchSequenceHandler : public ISystem {
public:
    explicit LaunchSequenceHandler(SimulationEngine& engine);
    ~LaunchSequenceHandler();

    // Request functions for UI button presses
    void requestAuthorization();
    void submitAuthorization(const std::string& inputCode);
    void requestArm();
    void requestLaunch();
    void requestReset();

    // Get current phase
    CurrentLaunchPhase getCurrentPhase() const;
    
    // Get current phase as string for display
    const char* getCurrentPhaseString() const;
    
    // Get the current authorization code for UI display
    const std::string& getAuthCode() const;
    
    // Check if authorization is pending (waiting for code input)
    bool isAuthorizationPending() const;
    
    // Force clear the authorization code (useful for external reset)
    void clearAuthCode();

    // ISystem interface implementation
    const char* getName() const override;
    void update(SimulationState& state, float dt) override;

    // Static methods to check boolean flags from SimulationState
    static bool checkTargetValidated(const SimulationState& state);
    static bool checkTargetAcquired(const SimulationState& state);
    static bool checkDepthClearanceMet(const SimulationState& state);
    static bool checkLaunchTubeIntegrity(const SimulationState& state);
    static bool checkPayloadSystemOperational(const SimulationState& state);
    static bool checkPowerSupplyStable(const SimulationState& state);
    static bool checkNoFriendlyUnitsInBlastRadius(const SimulationState& state);
    static bool checkLaunchConditionsFavorable(const SimulationState& state);
    
private:
    CurrentLaunchPhase currentPhase;
    SimulationEngine& engine;
    std::string authCode; // Stores the generated 4-digit authorization code
    float resetTimer; // Timer for reset state
    float armingTimer; // Timer for arming state
};
