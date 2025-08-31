#include <gtest/gtest.h>
#include "sim/systems/TargetingSystem.h"
#include "sim/SimulationState.h"

class TargetingSystemTest : public ::testing::Test {
protected:
    TargetingSystem targetingSystem;
    SimulationState state;
};

TEST_F(TargetingSystemTest, DefaultStability) {
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.5f);
}

TEST_F(TargetingSystemTest, StabilityAdjustment) {
    targetingSystem.adjustStability(0.2f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.7f);
    
    targetingSystem.adjustStability(-0.5f);
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.2f);
}

TEST_F(TargetingSystemTest, StabilityBounds) {
    targetingSystem.adjustStability(1.0f); // Should clamp to 1.0
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 1.0f);
    
    targetingSystem.adjustStability(-2.0f); // Should clamp to 0.0
    EXPECT_FLOAT_EQ(targetingSystem.getStability(), 0.0f);
}

TEST_F(TargetingSystemTest, UpdatesSimulationState) {
    targetingSystem.adjustStability(0.3f); // Set to 0.8
    targetingSystem.update(state, 0.1f);
    
    EXPECT_FLOAT_EQ(state.targetingStability, 0.8f);
}
