#include <gtest/gtest.h>
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "sim/systems/LaunchSequenceHandler/CurrentLaunchPhase.h"
#include "sim/SimulationEngine.h"
#include "sim/systems/MissileSystem.h"
#include "sim/systems/PowerSystem.h"
#include "sim/world/MissileManager.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"

class LaunchSequenceHandlerTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<LaunchSequenceHandler> handler;
    std::unique_ptr<PowerSystem> powerSystem;
    std::unique_ptr<MissileSystem> missileSystem;
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<MissileManager> missileManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        handler = std::make_unique<LaunchSequenceHandler>(*engine);
        powerSystem = std::make_unique<PowerSystem>();
        contactManager = std::make_unique<ContactManager>();
        missileManager = std::make_unique<MissileManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        missileSystem = std::make_unique<MissileSystem>(*missileManager, *contactManager, *crosshairManager);
        
        // Connect systems to handler
        handler->setMissileSystem(missileSystem.get());
        handler->setPowerSystem(powerSystem.get());
        
        // Register systems with engine
        engine->registerSystem(handler.get());
        engine->registerSystem(powerSystem.get());
        engine->registerSystem(missileSystem.get());
    }
    
    void setupLaunchConditions() {
        auto& state = engine->getState();
        state.targetValidated = true;
        state.targetAcquired = true;
        state.depthClearanceMet = true;
        state.launchTubeIntegrity = true;
        state.powerSupplyStable = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
        state.canLaunchAuthorized = true;
    }
};

TEST_F(LaunchSequenceHandlerTest, InitializesInIdlePhase) {
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_STREQ(handler->getCurrentPhaseString(), "Idle");
}

TEST_F(LaunchSequenceHandlerTest, AuthorizationRequestGeneratesCode) {
    setupLaunchConditions();
    
    handler->requestAuthorization();
    
    // Should transition to Authorized phase and generate auth code
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    EXPECT_FALSE(handler->getAuthCode().empty());
    EXPECT_EQ(handler->getAuthCode().length(), 4);
    EXPECT_TRUE(handler->isAuthorizationPending());
}

TEST_F(LaunchSequenceHandlerTest, AuthCodeSubmissionSuccess) {
    setupLaunchConditions();
    
    handler->requestAuthorization();
    std::string correctCode = handler->getAuthCode();
    
    handler->submitAuthorization(correctCode);
    
    // Should no longer be pending authorization
    EXPECT_FALSE(handler->isAuthorizationPending());
    // Should remain in Authorized phase, ready for arming
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Authorized);
}

TEST_F(LaunchSequenceHandlerTest, AuthCodeSubmissionFailure) {
    setupLaunchConditions();
    
    handler->requestAuthorization();
    std::string correctCode = handler->getAuthCode();
    std::string wrongCode = "0000"; // Unlikely to match random generation
    if (wrongCode == correctCode) wrongCode = "1111"; // Edge case
    
    handler->submitAuthorization(wrongCode);
    
    // Should still be pending authorization
    EXPECT_TRUE(handler->isAuthorizationPending());
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Authorized);
}

TEST_F(LaunchSequenceHandlerTest, ArmingSequence) {
    setupLaunchConditions();
    
    // Complete authorization
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    
    // Request arming
    handler->requestArm();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Arming);
    
    // Simulate time passing during arming (should take ~2 seconds)
    for (int i = 0; i < 150; i++) { // 150 frames at 60fps ≈ 2.5 seconds
        engine->update(0.016f);
    }
    
    // Should transition to Armed phase
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Armed);
    EXPECT_TRUE(engine->getState().payloadSystemOperational);
}

TEST_F(LaunchSequenceHandlerTest, LaunchSequence) {
    setupLaunchConditions();
    
    // Complete full sequence to Armed phase
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    // Complete arming
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Armed);
    
    // Request launch
    handler->requestLaunch();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Launching);
    
    // Complete launching (should take ~1 second)
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    // Should transition to Launched phase
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Launched);
    EXPECT_TRUE(engine->getState().missileLaunched);
}

TEST_F(LaunchSequenceHandlerTest, SystemReset) {
    setupLaunchConditions();
    
    // Get to Armed phase
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Armed);
    
    // Request reset
    handler->requestReset();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Resetting);
    
    // Complete reset (should take ~1 second)
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    // Should return to Idle phase
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_TRUE(handler->getAuthCode().empty());
}

TEST_F(LaunchSequenceHandlerTest, PreventUnauthorizedActions) {
    // Don't set up launch conditions - test safety checks
    
    // Should not authorize without conditions
    handler->requestAuthorization();
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    
    // Should not arm from idle
    handler->requestArm();
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    
    // Should not launch from idle
    handler->requestLaunch();
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
}

TEST_F(LaunchSequenceHandlerTest, RequiresPowerForOperation) {
    auto& state = engine->getState();
    state.targetValidated = true;
    state.targetAcquired = true;
    state.depthClearanceMet = true;
    state.launchTubeIntegrity = true;
    // Deliberately not setting powerSupplyStable
    state.powerSupplyStable = false;
    state.noFriendlyUnitsInBlastRadius = true;
    state.launchConditionsFavorable = true;
    
    handler->requestAuthorization();
    
    // Should not authorize without stable power
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
}

TEST_F(LaunchSequenceHandlerTest, AutoResetAfterLaunch) {
    setupLaunchConditions();
    
    // Complete full launch sequence
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    // Complete arming
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    handler->requestLaunch();
    
    // Complete launching
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Launched);
    
    // Wait for auto-reset (should happen after 2 seconds)
    for (int i = 0; i < 130; i++) {
        engine->update(0.016f);
    }
    
    // Should auto-reset to Idle
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
}
