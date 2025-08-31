#include <gtest/gtest.h>
#include "sim/SimulationEngine.h"
#include "sim/systems/PowerSystem.h"
#include "sim/systems/DepthSystem.h"
#include "sim/systems/SonarSystem.h"
#include "sim/systems/TargetingSystem.h"
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "sim/systems/EnvironmentSystem.h"
#include "sim/systems/TargetAcquisitionSystem.h"
#include "sim/systems/TargetValidationSystem.h"
#include "sim/systems/FriendlySafetySystem.h"
#include "sim/systems/MissileSystem.h"
#include "sim/world/ContactManager.h"
#include "sim/world/MissileManager.h"
#include "sim/world/CrosshairManager.h"
#include "ui/views/ControlPanel/ControlPanel.h"
#include <raylib.h>
#include <memory>

/**
 * Integration Test: Complete Launch Sequence
 * 
 * This test verifies the entire end-to-end launch sequence, ensuring that:
 * 1. UI components correctly communicate with backend systems
 * 2. Launch sequence phases work correctly
 * 3. Safety checks prevent unauthorized launches
 * 4. Missile system properly launches and tracks targets
 * 5. All systems integrate properly
 */
class LaunchSequenceIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<ContactManager> contacts;
    std::unique_ptr<MissileManager> missiles;
    std::unique_ptr<CrosshairManager> crosshairManager;
    
    // Systems
    std::shared_ptr<PowerSystem> power;
    std::shared_ptr<DepthSystem> depth;
    std::shared_ptr<SonarSystem> sonar;
    std::shared_ptr<TargetingSystem> targeting;
    std::shared_ptr<EnvironmentSystem> environment;
    std::shared_ptr<LaunchSequenceHandler> launchSequence;
    std::shared_ptr<TargetAcquisitionSystem> targetAcquisition;
    std::shared_ptr<TargetValidationSystem> targetValidation;
    std::shared_ptr<FriendlySafetySystem> friendlySafety;
    std::shared_ptr<MissileSystem> missileSystem;
    
    // UI Components
    std::unique_ptr<ControlPanel> controlPanel;
    
    void SetUp() override {
        // Initialize core components
        engine = std::make_unique<SimulationEngine>();
        contacts = std::make_unique<ContactManager>();
        missiles = std::make_unique<MissileManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contacts);
        
        // Initialize systems
        power = std::make_shared<PowerSystem>();
        depth = std::make_shared<DepthSystem>();
        sonar = std::make_shared<SonarSystem>(*contacts);
        targeting = std::make_shared<TargetingSystem>();
        environment = std::make_shared<EnvironmentSystem>();
        launchSequence = std::make_shared<LaunchSequenceHandler>(*engine);
        targetAcquisition = std::make_shared<TargetAcquisitionSystem>(*crosshairManager, *contacts);
        targetValidation = std::make_shared<TargetValidationSystem>(*crosshairManager, *contacts);
        friendlySafety = std::make_shared<FriendlySafetySystem>(*crosshairManager, *contacts);
        missileSystem = std::make_shared<MissileSystem>(*missiles, *contacts, *crosshairManager);
        
        // Register systems with engine
        engine->registerSystem(power);
        engine->registerSystem(depth);
        engine->registerSystem(sonar);
        engine->registerSystem(targeting);
        engine->registerSystem(environment);
        engine->registerSystem(launchSequence);
        engine->registerSystem(targetAcquisition);
        engine->registerSystem(targetValidation);
        engine->registerSystem(friendlySafety);
        engine->registerSystem(missileSystem);
        
        // Connect systems
        launchSequence->setMissileSystem(missileSystem.get());
        launchSequence->setPowerSystem(power.get());
        
        // Initialize UI
        controlPanel = std::make_unique<ControlPanel>(*engine, launchSequence.get());
        controlPanel->setBounds({640, 360, 620, 340});
        
        // Clear any existing state
        contacts->clearAllContacts();
    }
    
    void setupLaunchConditions() {
        // Set submarine to proper depth
        depth->setTargetDepth(-50.0f); // Operational depth
        
        // Turn on power
        power->setPowerState(true);
        
        // Spawn an enemy target
        uint32_t enemyId = contacts->spawnContact();
        
        // Update simulation to process initial states
        for (int i = 0; i < 10; i++) {
            engine->update(0.016f);
        }
        
        // Select the enemy target
        const auto& activeContacts = contacts->getActiveContacts();
        if (!activeContacts.empty()) {
            Vector2 targetPos = activeContacts[0].position;
            crosshairManager->setTargetingCirclePosition(targetPos);
            crosshairManager->trackContact(enemyId);
        }
        
        // Update again to process target acquisition
        for (int i = 0; i < 10; i++) {
            engine->update(0.016f);
        }
    }
    
    void simulateControlPanelClick(const std::string& buttonName) {
        // This would simulate clicking buttons in the control panel
        // For now, we'll directly call the launch sequence methods
        if (buttonName == "AUTHORIZE") {
            launchSequence->requestAuthorization();
        } else if (buttonName == "ARM") {
            launchSequence->requestArm();
        } else if (buttonName == "LAUNCH") {
            launchSequence->requestLaunch();
        } else if (buttonName == "RESET") {
            launchSequence->requestReset();
        }
    }
    
    void simulateAuthCodeEntry(const std::string& code) {
        launchSequence->submitAuthorization(code);
    }
};

TEST_F(LaunchSequenceIntegrationTest, CompleteSuccessfulLaunchSequence) {
    setupLaunchConditions();
    
    const auto& state = engine->getState();
    
    // Verify initial conditions are met
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_TRUE(state.depthClearanceMet);
    EXPECT_TRUE(state.targetAcquired);
    
    // Step 1: Request Authorization
    simulateControlPanelClick("AUTHORIZE");
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    EXPECT_TRUE(launchSequence->isAuthorizationPending());
    
    std::string authCode = launchSequence->getAuthCode();
    EXPECT_EQ(authCode.length(), 4);
    
    // Step 2: Enter Authorization Code
    simulateAuthCodeEntry(authCode);
    EXPECT_FALSE(launchSequence->isAuthorizationPending());
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    
    // Step 3: Arm System
    simulateControlPanelClick("ARM");
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Arming);
    
    // Wait for arming to complete (~2 seconds)
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Armed);
    EXPECT_TRUE(engine->getState().payloadSystemOperational);
    
    // Step 4: Launch Missile
    simulateControlPanelClick("LAUNCH");
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Launching);
    
    // Wait for launch to complete (~1 second)
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Launched);
    EXPECT_TRUE(engine->getState().missileLaunched);
    EXPECT_TRUE(engine->getState().missileActive);
    EXPECT_TRUE(missiles->isMissileActive());
    
    // Verify missile is tracking the target
    EXPECT_GT(engine->getState().missileTargetId, 0);
}

TEST_F(LaunchSequenceIntegrationTest, LaunchPreventedWithoutPower) {
    setupLaunchConditions();
    
    // Turn off power after setup
    power->setPowerState(false);
    engine->update(0.016f);
    
    EXPECT_FALSE(engine->getState().powerSupplyStable);
    
    // Try to authorize without power
    simulateControlPanelClick("AUTHORIZE");
    
    // Should remain in Idle phase
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(launchSequence->isAuthorizationPending());
}

TEST_F(LaunchSequenceIntegrationTest, LaunchPreventedWithoutTarget) {
    setupLaunchConditions();
    
    // Remove all targets
    contacts->clearAllContacts();
    crosshairManager->clearTarget();
    
    engine->update(0.016f);
    
    EXPECT_FALSE(engine->getState().targetAcquired);
    
    // Try to authorize without target
    simulateControlPanelClick("AUTHORIZE");
    
    // Should remain in Idle phase
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Idle);
}

TEST_F(LaunchSequenceIntegrationTest, WrongAuthCodePreventsArming) {
    setupLaunchConditions();
    
    // Request authorization
    simulateControlPanelClick("AUTHORIZE");
    EXPECT_TRUE(launchSequence->isAuthorizationPending());
    
    // Enter wrong code
    std::string wrongCode = "0000";
    std::string correctCode = launchSequence->getAuthCode();
    if (wrongCode == correctCode) wrongCode = "1111"; // Edge case
    
    simulateAuthCodeEntry(wrongCode);
    
    // Should still be pending authorization
    EXPECT_TRUE(launchSequence->isAuthorizationPending());
    
    // Try to arm - should not work
    simulateControlPanelClick("ARM");
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    EXPECT_NE(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Arming);
}

TEST_F(LaunchSequenceIntegrationTest, MissileExplodesWhenTargetLost) {
    setupLaunchConditions();
    
    // Complete launch sequence
    simulateControlPanelClick("AUTHORIZE");
    simulateAuthCodeEntry(launchSequence->getAuthCode());
    simulateControlPanelClick("ARM");
    
    // Complete arming
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    simulateControlPanelClick("LAUNCH");
    
    // Complete launching
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_TRUE(engine->getState().missileActive);
    EXPECT_TRUE(missiles->isMissileActive());
    
    // Remove the target (simulate destruction or loss)
    contacts->clearAllContacts();
    
    engine->update(0.016f);
    
    // Missile should explode and become inactive
    EXPECT_FALSE(engine->getState().missileActive);
    EXPECT_FALSE(missiles->isMissileActive());
}

TEST_F(LaunchSequenceIntegrationTest, SystemResetFromArmedState) {
    setupLaunchConditions();
    
    // Get to Armed state
    simulateControlPanelClick("AUTHORIZE");
    simulateAuthCodeEntry(launchSequence->getAuthCode());
    simulateControlPanelClick("ARM");
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Armed);
    
    // Reset system
    simulateControlPanelClick("RESET");
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Resetting);
    
    // Complete reset
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    // Should return to Idle
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_TRUE(launchSequence->getAuthCode().empty());
    EXPECT_FALSE(engine->getState().payloadSystemOperational);
}

TEST_F(LaunchSequenceIntegrationTest, PowerLossDuringSequence) {
    setupLaunchConditions();
    
    // Start launch sequence
    simulateControlPanelClick("AUTHORIZE");
    simulateAuthCodeEntry(launchSequence->getAuthCode());
    simulateControlPanelClick("ARM");
    
    // During arming, lose power
    for (int i = 0; i < 75; i++) { // Halfway through arming
        engine->update(0.016f);
    }
    
    power->setPowerState(false);
    
    // Continue simulation
    for (int i = 0; i < 75; i++) {
        engine->update(0.016f);
    }
    
    // System should handle power loss gracefully
    // (Exact behavior depends on implementation - test for no crashes)
    EXPECT_FALSE(engine->getState().powerSupplyStable);
}

TEST_F(LaunchSequenceIntegrationTest, AutoResetAfterSuccessfulLaunch) {
    setupLaunchConditions();
    
    // Complete full launch
    simulateControlPanelClick("AUTHORIZE");
    simulateAuthCodeEntry(launchSequence->getAuthCode());
    simulateControlPanelClick("ARM");
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    simulateControlPanelClick("LAUNCH");
    
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Launched);
    
    // Wait for auto-reset (should happen after 2 seconds)
    for (int i = 0; i < 130; i++) {
        engine->update(0.016f);
    }
    
    // Should auto-reset to Idle
    EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Idle);
}

TEST_F(LaunchSequenceIntegrationTest, MultipleLaunchCycles) {
    setupLaunchConditions();
    
    // Perform multiple launch cycles
    for (int cycle = 0; cycle < 3; cycle++) {
        // Ensure we have a target for each cycle
        if (contacts->getActiveContacts().empty()) {
            uint32_t enemyId = contacts->spawnContact();
            engine->update(0.016f);
            
            const auto& activeContacts = contacts->getActiveContacts();
            if (!activeContacts.empty()) {
                Vector2 targetPos = activeContacts[0].position;
                crosshairManager->setTargetingCirclePosition(targetPos);
                crosshairManager->trackContact(enemyId);
                engine->update(0.016f);
            }
        }
        
        // Complete launch sequence
        simulateControlPanelClick("AUTHORIZE");
        simulateAuthCodeEntry(launchSequence->getAuthCode());
        simulateControlPanelClick("ARM");
        
        for (int i = 0; i < 150; i++) {
            engine->update(0.016f);
        }
        
        simulateControlPanelClick("LAUNCH");
        
        for (int i = 0; i < 70; i++) {
            engine->update(0.016f);
        }
        
        EXPECT_TRUE(engine->getState().missileLaunched);
        
        // Wait for auto-reset
        for (int i = 0; i < 130; i++) {
            engine->update(0.016f);
        }
        
        EXPECT_EQ(launchSequence->getCurrentPhase(), CurrentLaunchPhase::Idle);
    }
}
