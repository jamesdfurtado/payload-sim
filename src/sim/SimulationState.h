#pragma once

struct SimulationState {
    // Core flags used across systems
    bool targetValidated = false;
    bool targetAcquired = false;
    bool depthClearanceMet = false;
    bool launchTubeIntegrity = true;
    bool payloadSystemOperational = false;
    bool powerSupplyStable = false;
    bool noFriendlyUnitsInBlastRadius = true;
    bool launchConditionsFavorable = true;

    // Derived flags
    bool canLaunchAuthorized = false;

    // Continuous state values managed by systems
    float currentDepthMeters;    // Managed by DepthSystem
    float powerLevel;            // Managed by PowerSystem (0.0-1.0 range)
    float targetingStability;    // Managed by TargetingSystem (0.0-1.0 range)
};


