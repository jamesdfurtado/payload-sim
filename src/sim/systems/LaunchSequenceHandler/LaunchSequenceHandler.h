#pragma once

#include "CurrentLaunchPhase.h"
#include "IdlePhase.h"
#include "AuthorizedPhase.h"
#include "../../SimulationState.h"
#include "../../ISystem.h"
#include <string>

class SimulationEngine; 
class MissileSystem;
class PowerSystem;

class LaunchSequenceHandler : public ISystem {
public:
    explicit LaunchSequenceHandler(SimulationEngine& engine);
    void setMissileSystem(MissileSystem* missileSystem) { this->missileSystem = missileSystem; }
    void setPowerSystem(PowerSystem* powerSystem) { this->powerSystem = powerSystem; }
    ~LaunchSequenceHandler();

    // phase transition requests from UI button presses
    void requestAuthorization();
    void submitAuthorization(const std::string& inputCode);
    void requestArm();
    void requestLaunch();
    void requestReset();

    CurrentLaunchPhase getCurrentPhase() const;
    const char* getCurrentPhaseString() const;
    const std::string& getAuthCode() const;
    bool isAuthorizationPending() const;
    void clearAuthCode();

    // ISystem interface implementation
    const char* getName() const override;
    void update(SimulationState& state, float dt) override;

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
    MissileSystem* missileSystem = nullptr;
    PowerSystem* powerSystem = nullptr;
    std::string authCode;
    float resetTimer;
    float armingTimer;
    float launchingTimer;
    float launchedTimer;
};
