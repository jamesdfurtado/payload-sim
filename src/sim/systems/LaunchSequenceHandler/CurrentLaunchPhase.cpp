#include "CurrentLaunchPhase.h"

const char* getCurrentPhaseString(CurrentLaunchPhase phase) {
    switch (phase) {
        case CurrentLaunchPhase::Idle:
            return "Idle";
        case CurrentLaunchPhase::Authorized:
            return "Authorized";
        case CurrentLaunchPhase::Armed:
            return "Armed";
        case CurrentLaunchPhase::Launched:
            return "Launched";
        default:
            return "Unknown";
    }
}
