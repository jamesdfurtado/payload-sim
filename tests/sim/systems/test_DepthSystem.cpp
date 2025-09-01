#include <gtest/gtest.h>
#include "sim/systems/DepthSystem.h"
#include "sim/SimulationState.h"

class DepthSystemTest : public ::testing::Test {
protected:
    DepthSystem depthSystem;
    SimulationState state;
    
    void SetUp() override {
        state = {};
    }
};

TEST_F(DepthSystemTest, InitializesWithRandomOptimalDepth) {
    float optimalDepth = depthSystem.getOptimalDepth();
    EXPECT_GE(optimalDepth, 50.0f);
    EXPECT_LE(optimalDepth, 200.0f);
}

TEST_F(DepthSystemTest, InitializesWithDepthNearOptimal) {
    float currentDepth = depthSystem.getDepth();
    float optimalDepth = depthSystem.getOptimalDepth();
    
    EXPECT_GE(currentDepth, 10.0f);
    EXPECT_LE(currentDepth, 500.0f);
    EXPECT_LE(abs(currentDepth - optimalDepth), 30.0f);
}

TEST_F(DepthSystemTest, UpdatesSimulationStateDepth) {
    depthSystem.update(state, 0.016f);
    EXPECT_EQ(state.currentDepthMeters, depthSystem.getDepth());
}

TEST_F(DepthSystemTest, DetectsClearanceWhenNearOptimalDepth) {
    float optimalDepth = depthSystem.getOptimalDepth();
    depthSystem.setDepth(optimalDepth);
    
    depthSystem.update(state, 0.016f);
    
    EXPECT_TRUE(state.depthClearanceMet);
}

TEST_F(DepthSystemTest, DetectsNoClearanceWhenFarFromOptimal) {
    float optimalDepth = depthSystem.getOptimalDepth();
    depthSystem.setDepth(optimalDepth + 20.0f);
    
    depthSystem.update(state, 0.016f);
    
    EXPECT_FALSE(state.depthClearanceMet);
}

TEST_F(DepthSystemTest, ClampsDepthToValidRange) {
    depthSystem.setDepth(1000.0f);
    EXPECT_EQ(depthSystem.getDepth(), 500.0f);
    
    depthSystem.setDepth(-50.0f);
    EXPECT_EQ(depthSystem.getDepth(), 10.0f);
}

TEST_F(DepthSystemTest, HandlesDepthChangeMovement) {
    float initialDepth = depthSystem.getDepth();
    depthSystem.setDepthChange(0.5f);
    
    depthSystem.update(state, 1.0f);
    
    EXPECT_GT(depthSystem.getDepth(), initialDepth);
    EXPECT_STREQ(depthSystem.getMovementStatus(), "ASCENDING");
}

TEST_F(DepthSystemTest, HandlesDescendingMovement) {
    float initialDepth = depthSystem.getDepth();
    depthSystem.setDepthChange(-0.5f);
    
    depthSystem.update(state, 1.0f);
    
    EXPECT_LT(depthSystem.getDepth(), initialDepth);
    EXPECT_STREQ(depthSystem.getMovementStatus(), "DESCENDING");
}

TEST_F(DepthSystemTest, HandlesHoldingPosition) {
    depthSystem.setDepthChange(0.0f);
    EXPECT_STREQ(depthSystem.getMovementStatus(), "HOLDING");
}

TEST_F(DepthSystemTest, ThrottleValueControlsDepthChange) {
    depthSystem.setThrottleValue(0.7f);
    float throttlePercentage = depthSystem.getThrottlePercentage();
    EXPECT_GT(throttlePercentage, 50.0f);
    
    depthSystem.setThrottleValue(0.3f);
    throttlePercentage = depthSystem.getThrottlePercentage();
    EXPECT_LT(throttlePercentage, 50.0f);
}

TEST_F(DepthSystemTest, ClampsDepthChangeToValidRange) {
    depthSystem.setDepthChange(2.0f);
    EXPECT_STREQ(depthSystem.getMovementStatus(), "ASCENDING");
    
    depthSystem.setDepthChange(-2.0f);
    EXPECT_STREQ(depthSystem.getMovementStatus(), "DESCENDING");
}

TEST_F(DepthSystemTest, ClearanceToleranceWorksCorrectly) {
    float optimalDepth = depthSystem.getOptimalDepth();
    
    depthSystem.setDepth(optimalDepth + 4.9f);
    depthSystem.update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    depthSystem.setDepth(optimalDepth - 4.9f);
    depthSystem.update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    depthSystem.setDepth(optimalDepth + 5.1f);
    depthSystem.update(state, 0.016f);
    EXPECT_FALSE(state.depthClearanceMet);
}
