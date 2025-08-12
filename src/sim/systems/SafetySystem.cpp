#include "SafetySystem.h"

bool SafetySystem::conditionsForAuthMet(const SimulationState& s) const {
    return s.targetValidated && s.targetAcquired && s.depthClearanceMet && s.launchTubeIntegrity &&
           s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

bool SafetySystem::conditionsForArmMet(const SimulationState& s) const {
    return conditionsForAuthMet(s) && s.canLaunchAuthorized;
}

bool SafetySystem::conditionsForLaunchMet(const SimulationState& s) const {
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.payloadSystemOperational && s.powerSupplyStable &&
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

bool SafetySystem::authorizedStateStillValid(const SimulationState& s) const {
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

bool SafetySystem::armedStateStillValid(const SimulationState& s) const {
    return s.canLaunchAuthorized && s.targetValidated && s.targetAcquired && s.depthClearanceMet &&
           s.launchTubeIntegrity && s.payloadSystemOperational && s.powerSupplyStable &&
           s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
}

void SafetySystem::update(SimulationState& state, float dt) {
    if (phase == LaunchPhase::Idle) {
        state.canLaunchAuthorized = false;
        state.payloadSystemOperational = false;
    }

    switch (phase) {
    case LaunchPhase::Idle: {
        resetReason.clear();
        if (eventAuthGranted && conditionsForAuthMet(state)) {
            state.canLaunchAuthorized = true;
            phase = LaunchPhase::Authorized;
        }
        break; }

    case LaunchPhase::Authorized: {
        state.canLaunchAuthorized = true;
        if (!authorizedStateStillValid(state)) {
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
            if (powerOffCallback) powerOffCallback();
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

void SafetySystem::arm() { eventArm = true; }
void SafetySystem::launch() { eventLaunch = true; }
void SafetySystem::reset() { phase = LaunchPhase::Resetting; resetTimer = 2.0f; }
void SafetySystem::forceIdle() { phase = LaunchPhase::Idle; }


