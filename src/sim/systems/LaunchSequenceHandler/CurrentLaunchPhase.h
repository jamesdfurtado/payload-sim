#pragma once

enum class CurrentLaunchPhase {
    Idle,
    Authorized,
    Armed,
    Launched,
    Resetting  // New transition state
};

// Helper functions for CurrentLaunchPhase
const char* getCurrentPhaseString(CurrentLaunchPhase phase);