#include <gtest/gtest.h>
#include "sim/systems/DepthSystem.h"
#include "sim/SimulationState.h"

class DepthSystemTest : public ::testing::Test {
protected:
    DepthSystem depthSystem;
    SimulationState state;
    
    void SetUp() override {
        state.currentDepthMeters = 0.0f; // Start at surface
        state.depthClearanceMet = false;
    }
};

TEST_F(DepthSystemTest, DefaultState) {
    EXPECT_FLOAT_EQ(state.currentDepthMeters, 0.0f);
    EXPECT_FALSE(state.depthClearanceMet);
    EXPECT_FLOAT_EQ(depthSystem.getTargetDepth(), 0.0f);
}

TEST_F(DepthSystemTest, ThrottleControl) {
    // Test throttle value setting and response
    depthSystem.setThrottle(0.8f); // 80% down throttle
    EXPECT_FLOAT_EQ(depthSystem.getThrottle(), 0.8f);
    
    // Update system and check depth changes
    float initialDepth = state.currentDepthMeters;
    depthSystem.update(state, 1.0f); // 1 second
    
    // Should be descending (more negative depth)
    EXPECT_LT(state.currentDepthMeters, initialDepth);
}

TEST_F(DepthSystemTest, DepthMovement) {
    // Test depth changes over time with target
    depthSystem.setTargetDepth(-50.0f);
    
    float initialDepth = state.currentDepthMeters;
    
    // Update for several frames
    for (int i = 0; i < 60; i++) { // 1 second at 60fps
        depthSystem.update(state, 0.016f);
    }
    
    // Should have moved toward target depth
    EXPECT_LT(state.currentDepthMeters, initialDepth);
    EXPECT_GT(state.currentDepthMeters, -50.0f); // Should be descending but not instantly there
}

TEST_F(DepthSystemTest, OptimalDepthCalculation) {
    // Test reaching optimal operational depth for launch
    depthSystem.setTargetDepth(-60.0f); // Operational depth
    
    // Simulate reaching target depth
    for (int i = 0; i < 300; i++) { // 5 seconds
        depthSystem.update(state, 0.016f);
    }
    
    // Should be at or near operational depth
    EXPECT_LT(state.currentDepthMeters, -50.0f);
    EXPECT_GT(state.currentDepthMeters, -70.0f);
}

TEST_F(DepthSystemTest, ClearanceValidation) {
    // Test depth clearance validation for launch safety
    
    // At surface - should not have clearance
    depthSystem.setTargetDepth(0.0f);
    depthSystem.update(state, 1.0f);
    EXPECT_FALSE(state.depthClearanceMet);
    
    // At shallow depth - should not have clearance
    depthSystem.setTargetDepth(-20.0f);
    for (int i = 0; i < 100; i++) {
        depthSystem.update(state, 0.016f);
    }
    EXPECT_FALSE(state.depthClearanceMet);
    
    // At operational depth - should have clearance
    depthSystem.setTargetDepth(-60.0f);
    for (int i = 0; i < 200; i++) {
        depthSystem.update(state, 0.016f);
    }
    EXPECT_TRUE(state.depthClearanceMet);
}

TEST_F(DepthSystemTest, EmergencySurfacing) {
    // Start at operational depth
    depthSystem.setTargetDepth(-70.0f);
    for (int i = 0; i < 200; i++) {
        depthSystem.update(state, 0.016f);
    }
    
    EXPECT_TRUE(state.depthClearanceMet);
    float deepDepth = state.currentDepthMeters;
    
    // Emergency surface
    depthSystem.setTargetDepth(0.0f);
    for (int i = 0; i < 300; i++) {
        depthSystem.update(state, 0.016f);
    }
    
    // Should have ascended and lost launch clearance
    EXPECT_GT(state.currentDepthMeters, deepDepth);
    EXPECT_FALSE(state.depthClearanceMet);
}

TEST_F(DepthSystemTest, ThrottleInfluencesDepthRate) {
    // Test that throttle affects rate of depth change
    float initialDepth = state.currentDepthMeters;
    
    // Test with low throttle
    depthSystem.setThrottle(0.2f);
    depthSystem.update(state, 1.0f);
    float lowThrottleDepth = state.currentDepthMeters;
    
    // Reset and test with high throttle
    state.currentDepthMeters = initialDepth;
    depthSystem.setThrottle(0.8f);
    depthSystem.update(state, 1.0f);
    float highThrottleDepth = state.currentDepthMeters;
    
    // High throttle should produce larger depth change
    float lowChange = abs(lowThrottleDepth - initialDepth);
    float highChange = abs(highThrottleDepth - initialDepth);
    EXPECT_GT(highChange, lowChange);
}

TEST_F(DepthSystemTest, DepthLimits) {
    // Test maximum depth limits
    depthSystem.setTargetDepth(-1000.0f); // Very deep
    
    for (int i = 0; i < 1000; i++) { // Long simulation
        depthSystem.update(state, 0.016f);
    }
    
    // Should not exceed reasonable submarine depth limits
    EXPECT_GT(state.currentDepthMeters, -300.0f); // Reasonable depth limit
}
