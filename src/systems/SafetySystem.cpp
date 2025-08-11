#include "SafetySystem.h"
#include <cstring>

bool SafetySystem::conditionsForAuthMet(const SimulationState& s) const {
    // All conditions except Authorization and Payload OK must be met
    return s.targetValidated && s.targetAcquired && s.depthClearanceMet && s.launchTubeIntegrity &&
           s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable; // payload gets set later
}

bool SafetySystem::conditionsForArmMet(const SimulationState& s) const {
    // All conditions except Payload OK must be met
    return conditionsForAuthMet(s) && s.canLaunchAuthorized;
}

bool SafetySystem::conditionsForLaunchMet(const SimulationState& s) const {
    // All conditions must be met
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.payloadSystemOperational && s.powerSupplyStable &&
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

bool SafetySystem::authorizedStateStillValid(const SimulationState& s) const {
    // While authorized, payload can still be false; everything else needs to stay true
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

bool SafetySystem::armedStateStillValid(const SimulationState& s) const {
    // Repeated check to ensure safe payload launch
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.payloadSystemOperational && s.powerSupplyStable &&
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

void SafetySystem::update(SimulationState& state, float dt) {
    // set system outputs based on the current safety phase
    // authorization to launch is only granted after successful authorization
    if (phase == LaunchPhase::Idle) {
        state.canLaunchAuthorized = false;
        state.payloadSystemOperational = false; // payload isn't armed here
    }

    switch (phase) {
    case LaunchPhase::Idle: {
        resetReason.clear();
        // Move to authorized if all conditions are met (safe)
        if (eventAuthGranted && conditionsForAuthMet(state)) {
            state.canLaunchAuthorized = true;
            phase = LaunchPhase::Authorized;
        }
        break; }

    case LaunchPhase::Authorized: {
        state.canLaunchAuthorized = true;
        if (!authorizedStateStillValid(state)) {
            // Message outputting what condition failed
            if (!state.targetValidated) resetReason = "Target lost validation";
            else if (!state.targetAcquired) resetReason = "Target lost";
            else if (!state.depthClearanceMet) resetReason = "Depth not safe";
            else if (!state.launchTubeIntegrity) resetReason = "Tube integrity lost";
            else if (!state.powerSupplyStable) resetReason = "Power unstable";
            else if (!state.noFriendlyUnitsInBlastRadius) resetReason = "Friendlies in blast radius";
            else if (!state.launchConditionsFavorable) resetReason = "Conditions unfavorable";
            else resetReason = "Unknown condition lost";
            phase = LaunchPhase::Resetting;
            resetTimer = 2.0f;
            break;
        }
        if (eventArm && conditionsForArmMet(state)) {
            armingTimer = 2.0f;
            phase = LaunchPhase::Arming;
        }
        break; }

    case LaunchPhase::Arming: {
        state.canLaunchAuthorized = true;
        armingTimer -= dt;
        if (armingTimer <= 0.0f) {
            state.payloadSystemOperational = true;
            phase = LaunchPhase::Armed;
        }
        break; }

    case LaunchPhase::Armed: {
        state.canLaunchAuthorized = true;
        state.payloadSystemOperational = true;
        if (!armedStateStillValid(state)) {
            // Error messages
            if (!state.targetValidated) resetReason = "Target lost validation";
            else if (!state.targetAcquired) resetReason = "Target lost";
            else if (!state.depthClearanceMet) resetReason = "Depth not safe";
            else if (!state.launchTubeIntegrity) resetReason = "Tube integrity lost";
            else if (!state.payloadSystemOperational) resetReason = "Payload not operational";
            else if (!state.powerSupplyStable) resetReason = "Power unstable";
            else if (!state.noFriendlyUnitsInBlastRadius) resetReason = "Friendlies in blast radius";
            else if (!state.launchConditionsFavorable) resetReason = "Conditions unfavorable";
            else resetReason = "Unknown condition lost";
            phase = LaunchPhase::Resetting;
            resetTimer = 2.0f;
            break;
        }
        if (eventLaunch && conditionsForLaunchMet(state)) {
            launchingTimer = 1.0f;
            phase = LaunchPhase::Launching;
        }
        break; }

    case LaunchPhase::Launching: {
        state.canLaunchAuthorized = true;
        state.payloadSystemOperational = true;
        launchingTimer -= dt;
        if (launchingTimer <= 0.0f) {
            phase = LaunchPhase::Launched;
            resetTimer = 2.0f;
        }
        break; }

    case LaunchPhase::Launched: {
        resetTimer -= dt;
        if (resetTimer <= 0.0f) {
            phase = LaunchPhase::Resetting;
            resetTimer = 2.0f;
        }
        break; }

    case LaunchPhase::Resetting: {
        state.canLaunchAuthorized = false;
        state.payloadSystemOperational = false;
        resetTimer -= dt;
        if (resetTimer <= 0.0f) {
            phase = LaunchPhase::Idle;
            resetReason.clear();
            
            // Turn off power when reset completes
            if (powerOffCallback) {
                powerOffCallback();
            }
        }
        break; }
    }

    eventAuthGranted = false;
    eventArm = false;
    eventLaunch = false;
}

void SafetySystem::requestAuthorizationCode(const char* code) {
    if (code && currentChallengeCode == code) {
        eventAuthGranted = true;
    }
}

void SafetySystem::arm() {
    eventArm = true;
}

void SafetySystem::launch() {
    eventLaunch = true;
}

void SafetySystem::reset() {
    phase = LaunchPhase::Resetting;
    resetTimer = 2.0f;
}

void SafetySystem::forceIdle() {
    phase = LaunchPhase::Idle;
}


