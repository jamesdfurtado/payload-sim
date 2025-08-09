#include <gtest/gtest.h>
#include "systems/SafetySystem.h"

class SafetySystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        safetySystem = std::make_unique<SafetySystem>();
        
        // Set up a baseline state with all conditions met except authorization
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

TEST_F(SafetySystemTest, InitialPhaseIsIdle) {
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, IdlePhaseSetsCorrectState) {
    safetySystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.canLaunchAuthorized);
    EXPECT_FALSE(state.payloadSystemOperational);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, AuthorizationRequiresCorrectCode) {
    // Set challenge code
    safetySystem->setChallengeCode("1234");
    
    // Wrong code should not authorize
    safetySystem->requestAuthorizationCode("5678");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    
    // Correct code should authorize if conditions are met
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    EXPECT_TRUE(state.canLaunchAuthorized);
}

TEST_F(SafetySystemTest, AuthorizationRequiresAllConditions) {
    safetySystem->setChallengeCode("1234");
    
    // Missing target validation
    state.targetValidated = false;
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    
    // Restore target validation, remove power stability
    state.targetValidated = true;
    state.powerSupplyStable = false;
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, CanArmAfterAuthorization) {
    // First authorize
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    // Then arm
    safetySystem->arm();
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Arming);
    
    // Wait for arming to complete (up to ~3 seconds)
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

TEST_F(SafetySystemTest, CanLaunchAfterArmed) {
    // Go through authorization and arming
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    safetySystem->arm();
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Arming);
    
    // Wait for arming to complete (2 seconds)
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Armed);
    EXPECT_TRUE(state.payloadSystemOperational);
    
    // Launch and tick until we transition
    safetySystem->launch();
    safetySystem->update(state, 0.016f);
    // Ensure we enter Launching
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
    
    // Wait for launch to complete (up to ~2 seconds)
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

TEST_F(SafetySystemTest, ResetsOnConditionLoss) {
    // Go to authorized state
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    // Lose a condition
    state.targetValidated = false;
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Resetting);
    EXPECT_EQ(safetySystem->getResetReason(), "Target lost validation");
    
    // Wait for reset to complete
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}

TEST_F(SafetySystemTest, ManualResetWorks) {
    // Go to any state
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    safetySystem->update(state, 0.016f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    // Manual reset
    safetySystem->reset();
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Resetting);
    
    // Wait for reset to complete
    safetySystem->update(state, 2.1f);
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
}
