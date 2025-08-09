#include <gtest/gtest.h>
#include "systems/SafetySystem.h"

class SafetySystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        safetySystem = std::make_unique<SafetySystem>();
        
        state.canLaunchAuthorized = false;
        state.targetValidated = true;
        state.targetAcquired = true;
        state.depthClearanceMet = true;
        state.launchTubeIntegrity = true;
        state.payloadSystemOperational = true;
        state.powerSupplyStable = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
    }

    std::unique_ptr<SafetySystem> safetySystem;
    SimulationState state;
};

TEST_F(SafetySystemTest, InitialPhaseIsIdle) { // Safety system starts in idle phase
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, IdlePhaseSetsCorrectState) { // Idle phase disables authorization and payload
    safetySystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.canLaunchAuthorized);
    EXPECT_FALSE(state.payloadSystemOperational);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, AuthorizationRequiresCorrectCode) { // Only correct auth code grants authorization
    safetySystem->setChallengeCode("1234");
    
    safetySystem->requestAuthorizationCode("5678");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    EXPECT_TRUE(state.canLaunchAuthorized);
}

TEST_F(SafetySystemTest, AuthorizationRequiresAllConditions) { // Authorization fails if any condition is missing
    safetySystem->setChallengeCode("1234");
    
    state.targetValidated = false;
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    
    state.targetValidated = true;
    state.powerSupplyStable = false;
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, CanArmAfterAuthorization) { // Can arm payload after authorization
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    safetySystem->arm();
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Arming);
    
    {
        float elapsed = 0.0f;
        for (int i = 0; i < 200 && safetySystem->getPhase() == LaunchPhase::Arming; ++i) {
            safetySystem->update(state, 0.016f);
            elapsed += 0.016f;
        }
        EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Armed);
        (void)elapsed;
    }
    EXPECT_TRUE(state.payloadSystemOperational);
}

TEST_F(SafetySystemTest, CanLaunchAfterArmed) { // Can launch payload after arming
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    safetySystem->arm();
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Arming);
    
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Armed);
    EXPECT_TRUE(state.payloadSystemOperational);
    
    safetySystem->launch();
    safetySystem->update(state, 0.016f);
    {
        float elapsed = 0.0f;
        for (int i = 0; i < 60 && safetySystem->getPhase() == LaunchPhase::Armed; ++i) {
            safetySystem->update(state, 0.016f);
            elapsed += 0.016f;
        }
        EXPECT_NE(safetySystem->getPhase(), LaunchPhase::Armed);
        (void)elapsed;
    }
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Launching);
    
    {
        float elapsed = 0.0f;
        for (int i = 0; i < 200 && safetySystem->getPhase() != LaunchPhase::Launched; ++i) {
            safetySystem->update(state, 0.016f);
            elapsed += 0.016f;
        }
        EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Launched);
        (void)elapsed;
    }
}

TEST_F(SafetySystemTest, ResetsOnConditionLoss) { // System resets when conditions are lost
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    state.targetValidated = false;
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Resetting);
    EXPECT_EQ(safetySystem->getResetReason(), "Target lost validation");
    
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, ManualResetWorks) { // Manual reset returns system to idle
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    safetySystem->reset();
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Resetting);
    
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}
