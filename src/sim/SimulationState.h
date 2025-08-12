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

    // Example continuous state
    float currentDepthMeters = 100.0f;
    float powerLevel = 0.0f; // 0..1
    float targetingStability = 0.5f; // 0..1
};


