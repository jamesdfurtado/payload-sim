#include "LaunchSequenceHandler.h"
#include "IdlePhase.h"
#include "AuthorizedPhase.h"
#include "ResettingPhase.h"
#include "../../SimulationEngine.h"
#include <iostream>
#include <string>

LaunchSequenceHandler::LaunchSequenceHandler(SimulationEngine& engine) 
    : engine(engine), currentPhase(CurrentLaunchPhase::Idle), authCode(""), resetTimer(0.0f) {
}

LaunchSequenceHandler::~LaunchSequenceHandler() {
}

void LaunchSequenceHandler::requestAuthorization() {
    std::cout << "[LaunchSequenceHandler] Authorization requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Idle) {
        // Check if we can authorize using IdlePhase validation with real state
        const auto& state = engine.getState();
        AuthorizationResult result = IdlePhase::canAuthorize(state);
        
        if (result.canAuthorize) {
            // Generate authorization code and store it
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
    
    if (authCode.empty()) {
        std::cout << "[LaunchSequenceHandler] No authorization code generated. Request authorization first." << std::endl;
        // Ensure we're in a clean idle state
        engine.getState().canLaunchAuthorized = false;
        return;
    }
    
    // Check if flags are still met (re-validate conditions)
    const auto& state = engine.getState();
    AuthorizationResult result = IdlePhase::canAuthorize(state);
    
    if (!result.canAuthorize) {
        std::cout << "[LaunchSequenceHandler] Authorization conditions no longer met: " << result.message << std::endl;
        authCode.clear(); // Clear the code since conditions changed
        // Reset to idle phase (already there, but ensure state is clean)
        engine.getState().canLaunchAuthorized = false;
        std::cout << "[LaunchSequenceHandler] Returning to idle phase due to condition failure" << std::endl;
        return;
    }
    
    // Check if the submitted code matches the generated code
    if (inputCode == authCode) {
        currentPhase = CurrentLaunchPhase::Authorized;
        engine.getState().canLaunchAuthorized = true;
        authCode.clear(); // Clear the code after successful authorization
        std::cout << "[LaunchSequenceHandler] Code verified successfully. Phase changed to: Authorized" << std::endl;
    } else {
        std::cout << "[LaunchSequenceHandler] Code verification failed. Expected: " << authCode 
                  << ", Received: " << inputCode << std::endl;
        authCode.clear(); // Clear the code after failed attempt
        // Reset to idle phase after wrong code input
        engine.getState().canLaunchAuthorized = false;
        std::cout << "[LaunchSequenceHandler] Returning to idle phase due to wrong code input" << std::endl;
    }
}

void LaunchSequenceHandler::requestArm() {
    std::cout << "[LaunchSequenceHandler] Arm requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Authorized) {
        // TODO: Implement arm validation logic
        // if that succeeds, change the phase to Armed
        currentPhase = CurrentLaunchPhase::Armed;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Armed" << std::endl;
    }
}

void LaunchSequenceHandler::requestLaunch() {
    std::cout << "[LaunchSequenceHandler] Launch requested" << std::endl;
    if (currentPhase == CurrentLaunchPhase::Armed) {
        // TODO: Implement launch validation logic
        // if that succeeds, change the phase to Launched
        currentPhase = CurrentLaunchPhase::Launched;
        std::cout << "[LaunchSequenceHandler] Phase changed to: Launched" << std::endl;
    }
}

void LaunchSequenceHandler::requestReset() {
    if (currentPhase != CurrentLaunchPhase::Idle) {
        std::cout << "[LaunchSequenceHandler] Manual reset requested, transitioning to reset phase" << std::endl;
        currentPhase = CurrentLaunchPhase::Resetting;
        resetTimer = 0.0f;
        engine.getState().canLaunchAuthorized = false;
        // Clear any pending authorization code
        authCode.clear();
    }
}

CurrentLaunchPhase LaunchSequenceHandler::getCurrentPhase() const {
    return currentPhase;
}

const char* LaunchSequenceHandler::getCurrentPhaseString() const {
    switch (currentPhase) {
        case CurrentLaunchPhase::Idle: return "Idle";
        case CurrentLaunchPhase::Authorized: return "Authorized";
        case CurrentLaunchPhase::Armed: return "Armed";
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
    // Handle reset state timing
    if (currentPhase == CurrentLaunchPhase::Resetting) {
        resetTimer += dt;
        
        if (ResettingPhase::isResetComplete(resetTimer)) {
            // Reset complete, transition to Idle
            currentPhase = CurrentLaunchPhase::Idle;
            resetTimer = 0.0f;
            state.canLaunchAuthorized = false;
            std::cout << "[LaunchSequenceHandler] Reset complete, now in Idle state" << std::endl;
        }
        return; // Don't process other logic while resetting
    }
    
    // Continuous monitoring: if we're in Authorized phase, check if we can stay authorized
    if (currentPhase == CurrentLaunchPhase::Authorized) {
        CheckAuthorizationStatus authStatus = AuthorizedPhase::canStayAuthorized(state);
        
        if (!authStatus.isAuthorized) {
            // Conditions failed - transition to resetting phase
            std::cout << "[LaunchSequenceHandler] Authorization conditions failed during monitoring: " 
                      << authStatus.message << std::endl;
            std::cout << "[LaunchSequenceHandler] Transitioning to reset phase due to condition failure" << std::endl;
            
            currentPhase = CurrentLaunchPhase::Resetting;
            resetTimer = 0.0f;
            state.canLaunchAuthorized = false;
        }
    }
}

// Static methods to check boolean flags from SimulationState
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
