#pragma once

#include "CurrentLaunchPhase.h"
#include "IdlePhase.h"

class SimulationEngine; // forward declaration

class LaunchSequenceHandler {
public:
    explicit LaunchSequenceHandler(SimulationEngine& engine);
    ~LaunchSequenceHandler();

    // Request functions for UI button presses
    void requestAuthorization();
    void requestArm();
    void requestLaunch();
    void requestReset();

    // Get current phase
    CurrentLaunchPhase getCurrentPhase() const;
    
    // Get current phase as string for display
    const char* getCurrentPhaseString() const;
    
private:
    CurrentLaunchPhase currentPhase;
    SimulationEngine& engine;
};
