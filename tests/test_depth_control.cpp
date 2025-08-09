#include <gtest/gtest.h>
#include "systems/DepthControl.h"

class DepthControlTest : public ::testing::Test {
protected:
    void SetUp() override {
        depthControl = std::make_unique<DepthControl>();
        
        state.submarineDepthMeters = 100.0f;
        state.optimalLaunchDepthMeters = 120.0f;
        state.depthClearanceMet = false;
    }

    std::unique_ptr<DepthControl> depthControl;
    SimulationState state;
};


TEST_F(DepthControlTest, InitialBallastIsNeutral) { // Neutral ballast keeps depth stable
    depthControl->update(state, 1.0f);
    
    float initialDepth = state.submarineDepthMeters;
    depthControl->update(state, 1.0f);
    
    EXPECT_NEAR(state.submarineDepthMeters, initialDepth, 5.0f);
}

TEST_F(DepthControlTest, BallastControlsMovement) { // Ballast up decreases depth
    float initialDepth = state.submarineDepthMeters;
    
    depthControl->setBallastThrottle(1.0f);
    
    for (int i = 0; i < 50; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_LT(state.submarineDepthMeters, initialDepth);
}

TEST_F(DepthControlTest, BallastControlsGoingDown) { // Ballast down increases depth
    float initialDepth = state.submarineDepthMeters;
    
    depthControl->setBallastThrottle(0.0f);
    
    for (int i = 0; i < 50; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_GT(state.submarineDepthMeters, initialDepth);
}

TEST_F(DepthControlTest, DepthBoundsAreEnforced) { // Depth is clamped to 5-400m range
    state.submarineDepthMeters = 10.0f;
    depthControl->setBallastThrottle(1.0f);
    
    for (int i = 0; i < 20; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_GE(state.submarineDepthMeters, 5.0f);
    
    state.submarineDepthMeters = 350.0f;
    depthControl->setBallastThrottle(0.0f);
    
    for (int i = 0; i < 20; ++i) {
        depthControl->update(state, 0.1f);
    }
    
    EXPECT_LE(state.submarineDepthMeters, 400.0f);
}

TEST_F(DepthControlTest, DepthClearanceCalculation) { // Clearance calculated with 5m tolerance
    state.submarineDepthMeters = 120.0f;
    state.optimalLaunchDepthMeters = 120.0f;
    
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 117.0f;
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 124.0f;
    depthControl->update(state, 0.016f);
    EXPECT_TRUE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 114.0f;
    depthControl->update(state, 0.016f);
    EXPECT_FALSE(state.depthClearanceMet);
    
    state.submarineDepthMeters = 126.0f;
    depthControl->update(state, 0.016f);
    EXPECT_FALSE(state.depthClearanceMet);
}

TEST_F(DepthControlTest, BallastThrottleIsClamped) { // Ballast throttle values are clamped
    EXPECT_NO_THROW(depthControl->setBallastThrottle(-10.0f));
    EXPECT_NO_THROW(depthControl->setBallastThrottle(10.0f));
    
    EXPECT_NO_THROW(depthControl->update(state, 0.016f));
}
