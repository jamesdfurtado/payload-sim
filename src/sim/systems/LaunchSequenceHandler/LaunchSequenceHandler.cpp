#include "LaunchSequenceHandler.h"
#include "IdlePhase.h"
#include "AuthorizedPhase.h"
#include "../../SimulationEngine.h"
#include <iostream>
#include <string>

LaunchSequenceHandler::LaunchSequenceHandler(SimulationEngine& engine) 
    : engine(engine), currentPhase(CurrentLaunchPhase::Idle), authCode("") {
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
    std::cout << "[LaunchSequenceHandler] Reset requested" << std::endl;
    currentPhase = CurrentLaunchPhase::Idle;
    // Reset the canLaunchAuthorized flag to false when returning to Idle
    engine.getState().canLaunchAuthorized = false;
    // Clear any pending authorization code
    authCode.clear();
    std::cout << "[LaunchSequenceHandler] Phase reset to: Idle, auth code cleared" << std::endl;
}

CurrentLaunchPhase LaunchSequenceHandler::getCurrentPhase() const {
    return currentPhase;
}

const char* LaunchSequenceHandler::getCurrentPhaseString() const {
    return ::getCurrentPhaseString(currentPhase);
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
    // Continuous monitoring: if we're in Authorized phase, check if we can stay authorized
    if (currentPhase == CurrentLaunchPhase::Authorized) {
        CheckAuthorizationStatus authStatus = AuthorizedPhase::canStayAuthorized(state);
        
        if (!authStatus.isAuthorized) {
            // Conditions failed - reset to idle phase
            std::cout << "[LaunchSequenceHandler] Authorization conditions failed during monitoring: " 
                      << authStatus.message << std::endl;
            std::cout << "[LaunchSequenceHandler] Resetting to idle phase due to condition failure" << std::endl;
            
            currentPhase = CurrentLaunchPhase::Idle;
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
