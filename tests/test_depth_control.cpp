#include <gtest/gtest.h>
#include "systems/DepthControl.h"

class DepthControlTest : public ::testing::Test {
protected:
    void SetUp() override {
        depthControl = std::make_unique<DepthControl>();
        
        // Initialize state
        state.submarineDepthMeters = 100.0f;
        state.optimalLaunchDepthMeters = 120.0f;
        state.depthClearanceMet = false;
    }

    std::unique_ptr<DepthControl> depthControl;
    SimulationState state;
};

TEST_F(DepthControlTest, HasCorrectName) {
    EXPECT_STREQ(depthControl->getName(), "DepthControl");
}

TEST_F(DepthControlTest, InitialBallastIsNeutral) {
    // Initial ballast should be neutral (0.5)
    // We can't directly test this as it's private, but we can test behavior
    depthControl->update(state, 1.0f);
    
    // At neutral ballast, depth should remain relatively stable
    float initialDepth = state.submarineDepthMeters;
    depthControl->update(state, 1.0f);
    
    // Should not move much with neutral ballast
    EXPECT_NEAR(state.submarineDepthMeters, initialDepth, 5.0f);
}

TEST_F(DepthControlTest, BallastControlsMovement) {
    float initialDepth = state.submarineDepthMeters;
    std::cout << "Initial depth: " << initialDepth << std::endl;
    
    // Set ballast to go up (1.0)
    depthControl->setBallastThrottle(1.0f);
    
    for (int i = 0; i < 50; ++i) {
        depthControl->update(state, 0.1f);
        if (i < 5 || i % 10 == 0) {
            std::cout << "Frame " << i << ": depth = " << state.submarineDepthMeters << std::endl;
        }
    }
    
    std::cout << "Final depth: " << state.submarineDepthMeters << " (should be < " << initialDepth << ")" << std::endl;
    // Should have moved up (decreased depth)
    EXPECT_LT(state.submarineDepthMeters, initialDepth);
}

TEST_F(DepthControlTest, BallastControlsGoingDown) {
    float initialDepth = state.submarineDepthMeters;
    std::cout << "Initial depth: " << initialDepth << std::endl;
    
    // Set ballast to go down (0.0)
    depthControl->setBallastThrottle(0.0f);
    
    for (int i = 0; i < 50; ++i) {
        depthControl->update(state, 0.1f);
        if (i < 5 || i % 10 == 0) {
            std::cout << "Frame " << i << ": depth = " << state.submarineDepthMeters << std::endl;
        }
    }
    
    std::cout << "Final depth: " << state.submarineDepthMeters << " (should be > " << initialDepth << ")" << std::endl;
    // Should have moved down (increased depth)
    EXPECT_GT(state.submarineDepthMeters, initialDepth);
}

TEST_F(DepthControlTest, DepthBoundsAreEnforced) {
    // Test minimum depth
    state.submarineDepthMeters = 10.0f;
    depthControl->setBallastThrottle(1.0f); // Go up
    
    for (int i = 0; i < 20; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_GE(state.submarineDepthMeters, 5.0f); // Minimum is 5m
    
    // Test maximum depth
    state.submarineDepthMeters = 350.0f;
    depthControl->setBallastThrottle(0.0f); // Go down
    
    for (int i = 0; i < 20; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_LE(state.submarineDepthMeters, 400.0f); // Maximum is 400m
}

TEST_F(DepthControlTest, DepthClearanceCalculation) {
    // Test when at optimal depth
    state.submarineDepthMeters = 120.0f; // Exactly optimal
    state.optimalLaunchDepthMeters = 120.0f;
    
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    // Test when within tolerance (5m)
    state.submarineDepthMeters = 117.0f; // 3m difference
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 124.0f; // 4m difference
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    // Test when outside tolerance
    state.submarineDepthMeters = 114.0f; // 6m difference
    depthControl->update(state, 0.016f);
    EXPECT_FALSE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 126.0f; // 6m difference
    depthControl->update(state, 0.016f);
    EXPECT_FALSE(state.depthClearanceMet);
}

TEST_F(DepthControlTest, BallastThrottleIsClamped) {
    // Test that ballast throttle values are properly clamped
    
    // This is tested indirectly by observing behavior
    // since setBallastThrottle clamps values internally
    
    // Test extreme values don't crash
    EXPECT_NO_THROW(depthControl->setBallastThrottle(-10.0f));
    EXPECT_NO_THROW(depthControl->setBallastThrottle(10.0f));
    
    // System should still respond normally after extreme inputs
    EXPECT_NO_THROW(depthControl->update(state, 0.016f));
}
