#pragma once

#include "../ISystem.h"
#include "../SimulationEngine.h"
#include "LaunchSequenceHandler/CommonTypes.h"
#include "LaunchSequenceHandler/AuthorizationPhase.h"
#include <memory>
#include <vector>
#include <string>

class LaunchSequenceHandler : public ISystem {
public:
    LaunchSequenceHandler(SimulationEngine& engine);
    ~LaunchSequenceHandler() = default;

    // ISystem interface
    const char* getName() const override { return "LaunchSequenceHandler"; }
    void update(SimulationState& state, float dt) override;

    // Main sequence control
    void startAuthorization();
    void arm();
    void launch();
    void reset();
    
    // State queries
    LaunchPhase getCurrentPhase() const { return currentPhase; }
    bool isAuthorized() const { return authorized; }
    
    // Authorization check
    AuthorizationResult checkLaunchAuthorization();

private:
    SimulationEngine& engine;
    LaunchPhase currentPhase;
    bool authorized;
    
    // Phase handlers
    std::unique_ptr<AuthorizationPhase> authorizationPhase;
    
    // Other phase handlers (to be implemented)
    // std::unique_ptr<IdlePhase> idlePhase;
    // std::unique_ptr<ArmingPhase> armingPhase;
    // std::unique_ptr<LaunchPhase> launchPhase;
    // std::unique_ptr<ResetPhase> resetPhase;
};
