#include <gtest/gtest.h>
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
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
        
        handler->setMissileSystem(missileSystem.get());
        handler->setPowerSystem(powerSystem.get());
        
        engine->registerSystem(handler.get());
        engine->registerSystem(powerSystem.get());
        engine->registerSystem(missileSystem.get());
    }
    
    void setupValidLaunchConditions() {
        auto& state = engine->getState();
        state.targetValidated = true;
        state.targetAcquired = true;
        state.depthClearanceMet = true;
        state.launchTubeIntegrity = true;
        state.powerSupplyStable = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
    }
};

TEST_F(LaunchSequenceHandlerTest, InitializesInIdlePhase) {
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(handler->isAuthorizationPending());
    EXPECT_TRUE(handler->getAuthCode().empty());
}

TEST_F(LaunchSequenceHandlerTest, CannotAuthorizeWithoutValidConditions) {
    handler->requestAuthorization();
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(handler->isAuthorizationPending());
    EXPECT_TRUE(handler->getAuthCode().empty());
}

TEST_F(LaunchSequenceHandlerTest, GeneratesAuthCodeWhenConditionsMet) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_TRUE(handler->isAuthorizationPending());
    EXPECT_FALSE(handler->getAuthCode().empty());
    EXPECT_EQ(handler->getAuthCode().length(), 4);
}

TEST_F(LaunchSequenceHandlerTest, TransitionsToAuthorizedWithCorrectCode) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    std::string correctCode = handler->getAuthCode();
    
    handler->submitAuthorization(correctCode);
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    EXPECT_FALSE(handler->isAuthorizationPending());
    EXPECT_TRUE(handler->getAuthCode().empty());
    EXPECT_TRUE(engine->getState().canLaunchAuthorized);
}

TEST_F(LaunchSequenceHandlerTest, RejectsIncorrectAuthCode) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    
    handler->submitAuthorization("0000");
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(handler->isAuthorizationPending());
    EXPECT_TRUE(handler->getAuthCode().empty());
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
}

TEST_F(LaunchSequenceHandlerTest, RevalidatesConditionsBeforeAuthorizing) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    std::string correctCode = handler->getAuthCode();
    
    engine->getState().powerSupplyStable = false;
    handler->submitAuthorization(correctCode);
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
}

TEST_F(LaunchSequenceHandlerTest, TransitionsFromAuthorizedToArming) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    
    handler->requestArm();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Arming);
}

TEST_F(LaunchSequenceHandlerTest, CompletesArmingAfterTime) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Armed);
    EXPECT_TRUE(engine->getState().payloadSystemOperational);
}

TEST_F(LaunchSequenceHandlerTest, TransitionsFromArmedToLaunching) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    handler->requestLaunch();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Launching);
}

TEST_F(LaunchSequenceHandlerTest, CompletesLaunchingAndTriggersMissile) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    handler->requestLaunch();
    
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Launched);
    EXPECT_TRUE(engine->getState().missileLaunched);
}

TEST_F(LaunchSequenceHandlerTest, AutoResetsAfterLaunch) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    handler->requestLaunch();
    
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    for (int i = 0; i < 130; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Idle);
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
    EXPECT_FALSE(engine->getState().payloadSystemOperational);
}

TEST_F(LaunchSequenceHandlerTest, ManualResetFromAnyPhase) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    
    handler->requestReset();
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Resetting);
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
    EXPECT_FALSE(engine->getState().payloadSystemOperational);
}

TEST_F(LaunchSequenceHandlerTest, MonitorsAuthorizedConditionsContinuously) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Authorized);
    
    engine->getState().targetValidated = false;
    engine->update(0.016f);
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Resetting);
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
}

TEST_F(LaunchSequenceHandlerTest, MonitorsArmedConditionsContinuously) {
    setupValidLaunchConditions();
    handler->requestAuthorization();
    handler->submitAuthorization(handler->getAuthCode());
    handler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Armed);
    
    engine->getState().noFriendlyUnitsInBlastRadius = false;
    engine->update(0.016f);
    
    EXPECT_EQ(handler->getCurrentPhase(), CurrentLaunchPhase::Resetting);
    EXPECT_FALSE(engine->getState().canLaunchAuthorized);
}
