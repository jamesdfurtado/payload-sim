#pragma once

#include <cstdint>

struct SimulationState {
    // targeting system state
    bool targetAcquired = false;
    bool targetValidated = false;
    float targetingStability;
    
    // missile system state
    bool missileLaunched = false;
    bool missileActive = false;
    bool explosionActive = false;
    uint32_t missileTargetId = 0;
    float explosionTimer = 0.0f;
    
    // power and systems status
    bool powerSupplyStable = false;
    bool payloadSystemOperational = false;
    bool launchTubeIntegrity = true;
    float powerLevel;
    
    // environment and safety checks
    bool depthClearanceMet = false;
    bool noFriendlyUnitsInBlastRadius = true;
    bool launchConditionsFavorable = true;
    float currentDepthMeters;
    
    // final launch authorization (derived from all the above)
    bool canLaunchAuthorized = false;
};


