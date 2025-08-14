enum class CurrentLaunchPhase {
    Idle,
    Authorized,
    Armed,
    Launched
};

// Helper functions for CurrentLaunchPhase
const char* getCurrentPhaseString(CurrentLaunchPhase phase);