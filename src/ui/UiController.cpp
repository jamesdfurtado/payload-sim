#include "UiController.h"
#include "../systems/SafetySystem.h"
#include "../systems/PowerSystem.h"

UiController::UiController(SimulationEngine& engine,
                           SonarSystem* sonar,
                           PowerSystem* power,
                           DepthControl* depth,
                           TargetingSystem* targeting,
                           SafetySystem* safety,
                           EnvironmentSystem* environment)
    : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety), environment(environment) {
    
    // Initialize UI components
    inputHandler = std::make_unique<InputHandler>(engine, sonar, power, depth, targeting, safety);
    statusPanel = std::make_unique<StatusPanel>(engine, power);
    sonarDisplay = std::make_unique<SonarDisplay>(engine, sonar, safety);
    interactiveControls = std::make_unique<InteractiveControls>(engine, safety, depth);
    loggingSystem = std::make_unique<LoggingSystem>();
    
    // Set up power off callback for when reset completes
    if (safety) {
        safety->setPowerOffCallback([this, power]() {
            if (power) {
                power->setPowerLevel(0.0f); // Turn power OFF
                uiPowerLevel = 0.0f; // Update UI power level
            }
        });
    }
}

void UiController::update(float dt) {
    // Update interactive controls (mouse-based UI with new auth calculator)
    interactiveControls->update(dt, uiState, authState, *loggingSystem);
    
    // Handle sonar clicks
    Rectangle sonarRect{ 20, 120, 600, 580 };
    inputHandler->handleSonarClick(sonarRect);
    
    // Update missile animation - now handled by ContactManager
    sonarDisplay->updateMissileAnimation(dt, missileState, sonarRect);
    
    // Handle targeting controls (Q/E keys still work)
    inputHandler->handleInput(dt, inputState);
    
    // Update logging system
    loggingSystem->update(dt);
    
    // Add phase-specific messages automatically
    if (safety) {
        switch (safety->getPhase()) {
            case LaunchPhase::Arming:
                loggingSystem->addPhaseMessage("Arming payload...");
                break;
            case LaunchPhase::Launching:
                loggingSystem->addPhaseMessage("Launching payload...");
                break;
            case LaunchPhase::Launched:
                loggingSystem->addPhaseMessage("Payload LAUNCHED!");
                break;
            case LaunchPhase::Resetting:
                if (!safety->getResetReason().empty()) {
                    loggingSystem->addPhaseMessage("System resetting", safety->getResetReason());
                } else {
                    loggingSystem->addPhaseMessage("System RESETTING...");
                }
                break;
        }
        
        // Check for authorization success
        if (safety->getPhase() == LaunchPhase::Authorized) {
            loggingSystem->addAuthSuccess();
        }
        
        // Check for payload armed
        if (safety->getPhase() == LaunchPhase::Armed) {
            loggingSystem->addPayloadArmed();
        }
        
        // Check for system reset - only when transitioning from Resetting to Idle
        static LaunchPhase previousPhase = LaunchPhase::Idle;
        if (previousPhase == LaunchPhase::Resetting && safety->getPhase() == LaunchPhase::Idle) {
            loggingSystem->addSystemReset();
        }
        
        previousPhase = safety->getPhase();
        
        // Set current payload state
        const char* phaseStr = "";
        switch (safety->getPhase()) {
            case LaunchPhase::Idle: phaseStr = "IDLE"; break;
            case LaunchPhase::Authorized: phaseStr = "AUTHORIZED"; break;
            case LaunchPhase::Arming: phaseStr = "ARMING"; break;
            case LaunchPhase::Armed: phaseStr = "ARMED"; break;
            case LaunchPhase::Launching: phaseStr = "LAUNCHING"; break;
            case LaunchPhase::Launched: phaseStr = "LAUNCHED"; break;
            case LaunchPhase::Resetting: phaseStr = "RESETTING"; break;
            default: phaseStr = "UNKNOWN"; break;
        }
        loggingSystem->setPayloadState(phaseStr);
    }
}

void UiController::render() {
    Rectangle status = { 640, 20, 620, 110 };
    Rectangle powerR = { 640, 140, 620, 110 };
    Rectangle depthR = { 640, 260, 620, 110 };
    Rectangle controls = { 640, 380, 620, 320 };
    Rectangle sonarR = { 20, 120, 600, 580 };

    DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE);

    // Delegate rendering to UI components
    statusPanel->drawStatusLights(status);
    statusPanel->drawPower(powerR, uiPowerLevel);
    statusPanel->drawDepth(depthR);
    interactiveControls->drawDepthControlInPanel(depthR, uiState);
    interactiveControls->drawInteractiveControls(controls, uiState, authState);
    
    // Draw centralized logging messages
    loggingSystem->drawMessages(controls);
    
    sonarDisplay->drawSonar(sonarR, missileState);
}




