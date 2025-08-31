#pragma once

enum class CurrentLaunchPhase {
    Idle,
    Authorized,
    Arming,
    Armed,
    Launching,
    Launched,
    Resetting
};

const char* getCurrentPhaseString(CurrentLaunchPhase phase);