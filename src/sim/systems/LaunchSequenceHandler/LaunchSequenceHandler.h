#pragma once

#include "CurrentLaunchPhase.h"

class LaunchSequenceHandler {
public:
    LaunchSequenceHandler();
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
};
