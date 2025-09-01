#include <gtest/gtest.h>
#include "sim/systems/TargetingSystem.h"
#include "sim/SimulationState.h"

class TargetingSystemTest : public ::testing::Test {
protected:
    TargetingSystem targetingSystem;
    SimulationState state;
    
    void SetUp() override {
        state = {};
    }
};

TEST_F(TargetingSystemTest, InitializesWithMidStability) {
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.5f);
}

TEST_F(TargetingSystemTest, UpdatesSimulationStateStability) {
    targetingSystem.update(state, 0.016f);
    EXPECT_FLOAT_EQ(state.targetingStability, 0.5f);
}

TEST_F(TargetingSystemTest, AdjustsStabilityUpward) {
    targetingSystem.adjustStability(0.3f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.8f);
    
    targetingSystem.update(state, 0.016f);
    EXPECT_FLOAT_EQ(state.targetingStability, 0.8f);
}

TEST_F(TargetingSystemTest, AdjustsStabilityDownward) {
    targetingSystem.adjustStability(-0.2f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.3f);
    
    targetingSystem.update(state, 0.016f);
    EXPECT_FLOAT_EQ(state.targetingStability, 0.3f);
}

TEST_F(TargetingSystemTest, ClampsStabilityToMaximum) {
    targetingSystem.adjustStability(0.8f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 1.0f);
    
    targetingSystem.update(state, 0.016f);
    EXPECT_FLOAT_EQ(state.targetingStability, 1.0f);
}

TEST_F(TargetingSystemTest, ClampsStabilityToMinimum) {
    targetingSystem.adjustStability(-0.8f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.0f);
    
    targetingSystem.update(state, 0.016f);
    EXPECT_FLOAT_EQ(state.targetingStability, 0.0f);
}

TEST_F(TargetingSystemTest, HandlesMultipleAdjustments) {
    targetingSystem.adjustStability(0.2f);
    targetingSystem.adjustStability(0.1f);
    targetingSystem.adjustStability(-0.4f);
    
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.4f);
}
