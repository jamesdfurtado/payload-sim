#include "LaunchSequenceHandler.h"

LaunchSequenceHandler::LaunchSequenceHandler(SimulationEngine& engine) 
    : engine(engine), currentPhase(LaunchPhase::Idle), authorized(false) {
    
    // Create phase handlers
    authorizationPhase = std::make_unique<AuthorizationPhase>();
}

void LaunchSequenceHandler::update(SimulationState& state, float dt) {
    // TODO: Implement system update logic
    // For now, this is just a placeholder to satisfy the ISystem interface
}

void LaunchSequenceHandler::startAuthorization() {
    if (currentPhase == LaunchPhase::Idle) {
        currentPhase = LaunchPhase::Authorization;
        
        // Get actual SimulationState from the engine
        SimulationState& state = engine.getState();
        
        // Check authorization using the AuthorizationPhase
        auto result = authorizationPhase->validateRequirements(state);
        
        if (result.success) {
            authorized = true;
            currentPhase = LaunchPhase::Idle; // Return to idle but now authorized
        } else {
            // Stay in Authorization phase if failed
            currentPhase = LaunchPhase::Authorization;
        }
    }
}

void LaunchSequenceHandler::arm() {
    if (authorized && currentPhase == LaunchPhase::Idle) {
        currentPhase = LaunchPhase::Arming;
        // TODO: Implement arming logic
    }
}

void LaunchSequenceHandler::launch() {
    if (currentPhase == LaunchPhase::Arming) {
        currentPhase = LaunchPhase::Launch;
        // TODO: Implement launch logic
        currentPhase = LaunchPhase::Launched;
    }
}

void LaunchSequenceHandler::reset() {
    currentPhase = LaunchPhase::Idle;
    authorized = false;
}

AuthorizationResult LaunchSequenceHandler::checkLaunchAuthorization() {
    // Get actual SimulationState from the engine
    SimulationState& state = engine.getState();
    
    return authorizationPhase->validateRequirements(state);
}
