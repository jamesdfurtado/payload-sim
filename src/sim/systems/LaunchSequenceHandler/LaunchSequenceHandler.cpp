#include "LaunchSequenceHandler.h"
#include "IdlePhase.h"
#include "AuthorizedPhase.h"
#include "ArmingPhase.h"
#include "ArmedPhase.h"
#include "LaunchingPhase.h"
#include "ResettingPhase.h"
#include "../../SimulationEngine.h"
#include "../PowerSystem.h"
#include <iostream>
#include <string>

LaunchSequenceHandler::LaunchSequenceHandler(SimulationEngine& engine) 
    : engine(engine), currentPhase(CurrentLaunchPhase::Idle), authCode(""), resetTimer(0.0f), armingTimer(0.0f), launchingTimer(0.0f), launchedTimer(0.0f) {
}

LaunchSequenceHandler::~LaunchSequenceHandler() {
}

void LaunchSequenceHandler::requestAuthorization() {
    std::cout << "[LaunchSequenceHandler] Authorization requested" << std::endl;
    
    // only allow authorization if in idle phase
    if (currentPhase == CurrentLaunchPhase::Idle) {
        const auto& state = engine.getState();
        AuthorizationResult result = IdlePhase::canAuthorize(state);
        
        if (result.canAuthorize) {
            // generate authorization code and store it
            authCode = IdlePhase::createCode();
            std::cout << "[LaunchSequenceHandler] Authorization conditions met. Generated code: " << authCode << std::endl;
            std::cout << "[LaunchSequenceHandler] " << result.message << std::endl;
            std::cout << "[LaunchSequenceHandler] Waiting for code submission..." << std::endl;
        } else {
            std::cout << "[LaunchSequenceHandler] Authorization denied: " << result.message << std::endl;
        }
    } else {
        std::cout << "[LaunchSequenceHandler] Cannot authorize from current phase: " 
                  << ::getCurrentPhaseString(currentPhase) << std::endl;
    }
}

void LaunchSequenceHandler::submitAuthorization(const std::string& inputCode) {
    std::cout << "[LaunchSequenceHandler] Code submission received: " << inputCode << std::endl;
    
    if (currentPhase != CurrentLaunchPhase::Idle) {
        std::cout << "[LaunchSequenceHandler] Cannot submit code from current phase: " 
                  << ::getCurrentPhaseString(currentPhase) << std::endl;
        return;
    }
    
    // auth code must be generated before submitting
    if (authCode.empty()) {
        std::cout << "[LaunchSequenceHandler] No authorization code generated. Request authorization first." << std::endl;
        engine.getState().canLaunchAuthorized = false;
        return;
    }
    
    // re-validate conditions before authorizing
    const auto& state = engine.getState();
    AuthorizationResult result = IdlePhase::canAuthorize(state);
    
    // reset auth process if can't authorize
    if (!result.canAuthorize) {
        std::cout << "[LaunchSequenceHandler] Authorization conditions no longer met: " << result.message << std::endl;
        authCode.clear();
        engine.getState().canLaunchAuthorized = false;
        std::cout << "[LaunchSequenceHandler] Returning to idle phase due to condition failure" << std::endl;
        return;
    }
    
    // check submitted code matches generated code
    if (inputCode == authCode) {
        currentPhase = CurrentLaunchPhase::Authorized;
        engine.getState().canLaunchAuthorized = true;
        authCode.clear(); 
        std::cout << "[LaunchSequenceHandler] Code verified successfully. Phase changed to: Authorized" << std::endl;
    } else {
        std::cout << "[LaunchSequenceHandler] Code verification failed. Expected: " << authCode 
                  << ", Received: " << inputCode << std::endl;
        authCode.clear(); 
        engine.getState().canLaunchAuthorized = false;
        std::cout << "[LaunchSequenceHandler] Returning to idle phase due to wrong code input" << std::endl;
    }
}

void LaunchSequenceHandler::requestArm() {
    std::cout << "[LaunchSequenceHandler] Arm requested" << std::endl;

    if (currentPhase == CurrentLaunchPhase::Authorized) {
        currentPhase = CurrentLaunchPhase::Arming;
        armingTimer = 0.0f;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Arming" << std::endl;
    }
}

void LaunchSequenceHandler::requestLaunch() {
    std::cout << "[LaunchSequenceHandler] Launch requested" << std::endl;

    if (currentPhase == CurrentLaunchPhase::Armed) {
        currentPhase = CurrentLaunchPhase::Launching;
        launchingTimer = 0.0f;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Launching" << std::endl;
    }
}

// handle manual reset of launch sequence
void LaunchSequenceHandler::requestReset() {
    if (currentPhase != CurrentLaunchPhase::Idle) {
        std::cout << "[LaunchSequenceHandler] Manual reset requested, transitioning to reset phase" << std::endl;
        currentPhase = CurrentLaunchPhase::Resetting;
        resetTimer = 0.0f;
        engine.getState().canLaunchAuthorized = false;
        engine.getState().payloadSystemOperational = false;
        authCode.clear();
        if (powerSystem) {
            powerSystem->setPowerState(false);
            std::cout << "[LaunchSequenceHandler] Power switch turned OFF during reset" << std::endl;
        }
    }
}

CurrentLaunchPhase LaunchSequenceHandler::getCurrentPhase() const {
    return currentPhase;
}

const char* LaunchSequenceHandler::getCurrentPhaseString() const {
    switch (currentPhase) {
        case CurrentLaunchPhase::Idle: return "Idle";
        case CurrentLaunchPhase::Authorized: return "Authorized";
        case CurrentLaunchPhase::Arming: return "Arming";
        case CurrentLaunchPhase::Armed: return "Armed";
        case CurrentLaunchPhase::Launching: return "Launching";
        case CurrentLaunchPhase::Launched: return "Launched";
        case CurrentLaunchPhase::Resetting: return "Resetting";
        default: return "Unknown";
    }
}

const std::string& LaunchSequenceHandler::getAuthCode() const {
    return authCode;
}

bool LaunchSequenceHandler::isAuthorizationPending() const {
    return !authCode.empty() && currentPhase == CurrentLaunchPhase::Idle;
}

void LaunchSequenceHandler::clearAuthCode() {
    authCode.clear();
    std::cout << "[LaunchSequenceHandler] Authorization code cleared externally" << std::endl;
}

// ISystem interface implementation
const char* LaunchSequenceHandler::getName() const {
    return "LaunchSequenceHandler";
}

void LaunchSequenceHandler::update(SimulationState& state, float dt) {
    // arming state timing (2 seconds)
    if (currentPhase == CurrentLaunchPhase::Arming) {
        armingTimer += dt;
        
        if (ArmingPhase::isArmingComplete(armingTimer)) {
            currentPhase = CurrentLaunchPhase::Armed;
            armingTimer = 0.0f;

            state.payloadSystemOperational = true;
            std::cout << "[LaunchSequenceHandler] Arming complete, now in Armed state" << std::endl;
        }
        return;
    }
    
    // launching state timing (1 second)
    if (currentPhase == CurrentLaunchPhase::Launching) {
        launchingTimer += dt;
        
        if (LaunchingPhase::isLaunchingComplete(launchingTimer)) {
            currentPhase = CurrentLaunchPhase::Launched;
            launchingTimer = 0.0f;
            launchedTimer = 0.0f;
            
            if (missileSystem) {
                state.missileLaunched = true;
                std::cout << "[LaunchSequenceHandler] Missile launch triggered" << std::endl;
            }
            
            std::cout << "[LaunchSequenceHandler] Launching complete, now in Launched state" << std::endl;
        }
        return;
    }
    
    // launched state timing (2 seconds)
    if (currentPhase == CurrentLaunchPhase::Launched) {
        launchedTimer += dt;
        
        if (launchedTimer >= 2.0f) { 
            // reset flow after launch
            currentPhase = CurrentLaunchPhase::Resetting;
            launchedTimer = 0.0f;
            resetTimer = 0.0f;

            state.payloadSystemOperational = false;
            std::cout << "[LaunchSequenceHandler] Launched state complete, transitioning to reset phase" << std::endl;
        }
        return;
    }
    
    // reset state timing (2 seconds)
    if (currentPhase == CurrentLaunchPhase::Resetting) {
        resetTimer += dt;
        
        if (ResettingPhase::isResetComplete(resetTimer)) {
            // reset phase-->idle phase
            currentPhase = CurrentLaunchPhase::Idle;
            resetTimer = 0.0f;
            state.canLaunchAuthorized = false;
            state.payloadSystemOperational = false;
            if (powerSystem) {
                powerSystem->setPowerState(false);
                std::cout << "[LaunchSequenceHandler] Power switch turned OFF when entering Idle state" << std::endl;
            }
            std::cout << "[LaunchSequenceHandler] Reset complete, now in Idle state" << std::endl;
        }
        return;
    }
    
    // continuous authorization surveillance
    if (currentPhase == CurrentLaunchPhase::Authorized) {
        CheckAuthorizationStatus authStatus = AuthorizedPhase::canStayAuthorized(state);
        
        if (!authStatus.isAuthorized) {
            std::cout << "[LaunchSequenceHandler] Authorization conditions failed during monitoring: " 
                      << authStatus.message << std::endl;
            std::cout << "[LaunchSequenceHandler] Transitioning to reset phase due to condition failure" << std::endl;
            
            currentPhase = CurrentLaunchPhase::Resetting;
            resetTimer = 0.0f;
            state.canLaunchAuthorized = false;
        }
    }
    
    // continuous armed surveillance
    if (currentPhase == CurrentLaunchPhase::Armed) {
        CheckAuthorizationStatus armedStatus = ArmedPhase::canStayArmed(state);
        
        if (!armedStatus.isAuthorized) {
            std::cout << "[LaunchSequenceHandler] Armed conditions failed during monitoring: " 
                      << armedStatus.message << std::endl;
            std::cout << "[LaunchSequenceHandler] Transitioning to reset phase due to armed condition failure" << std::endl;
            
            currentPhase = CurrentLaunchPhase::Resetting;
            resetTimer = 0.0f;
            state.canLaunchAuthorized = false;
        }
    }
}

// methods to check simulation state conditions
bool LaunchSequenceHandler::checkTargetValidated(const SimulationState& state) {
    return state.targetValidated;
}

bool LaunchSequenceHandler::checkTargetAcquired(const SimulationState& state) {
    return state.targetAcquired;
}

bool LaunchSequenceHandler::checkDepthClearanceMet(const SimulationState& state) {
    return state.depthClearanceMet;
}

bool LaunchSequenceHandler::checkLaunchTubeIntegrity(const SimulationState& state) {
    return state.launchTubeIntegrity;
}

bool LaunchSequenceHandler::checkPayloadSystemOperational(const SimulationState& state) {
    return state.payloadSystemOperational;
}

bool LaunchSequenceHandler::checkPowerSupplyStable(const SimulationState& state) {
    return state.powerSupplyStable;
}

bool LaunchSequenceHandler::checkNoFriendlyUnitsInBlastRadius(const SimulationState& state) {
    return state.noFriendlyUnitsInBlastRadius;
}

bool LaunchSequenceHandler::checkLaunchConditionsFavorable(const SimulationState& state) {
    return state.launchConditionsFavorable;
}
