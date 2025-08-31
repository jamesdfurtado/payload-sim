#include <gtest/gtest.h>
#include "sim/systems/EnvironmentSystem.h"
#include "sim/SimulationState.h"

class EnvironmentSystemTest : public ::testing::Test {
protected:
    EnvironmentSystem environmentSystem;
    SimulationState state;
    
    void SetUp() override {
        // Set default environmental conditions
        state.launchConditionsFavorable = true;
        state.noFriendlyUnitsInBlastRadius = true;
    }
};

TEST_F(EnvironmentSystemTest, InitializesCorrectly) {
    EXPECT_STREQ(environmentSystem.getName(), "EnvironmentSystem");
}

TEST_F(EnvironmentSystemTest, DefaultConditions) {
    EXPECT_TRUE(state.launchConditionsFavorable);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(EnvironmentSystemTest, EnvironmentalFactors) {
    // Test that environment system maintains favorable launch conditions
    EXPECT_TRUE(state.launchConditionsFavorable);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    
    // Update environment system
    environmentSystem.update(state, 0.016f);
    
    // Should maintain favorable conditions
    EXPECT_TRUE(state.launchConditionsFavorable);
    EXPECT_TRUE(state.launchTubeIntegrity);
}

TEST_F(EnvironmentSystemTest, WeatherImpact) {
    // Test weather impact simulation (currently favorable always)
    // Initial conditions should be favorable
    EXPECT_TRUE(state.launchConditionsFavorable);
    
    // Update multiple times - should remain favorable
    for (int i = 0; i < 100; i++) {
        environmentSystem.update(state, 0.016f);
        EXPECT_TRUE(state.launchConditionsFavorable);
    }
    
    // In future implementations, this could test adverse weather conditions
    // that would set launchConditionsFavorable to false
}

TEST_F(EnvironmentSystemTest, SafetyChecks) {
    // Test safety zone validation
    // Setup initial safe state
    state.noFriendlyUnitsInBlastRadius = true;
    
    // Update environment system
    environmentSystem.update(state, 0.016f);
    
    // Should maintain launch tube integrity
    EXPECT_TRUE(state.launchTubeIntegrity);
    
    // Test multiple updates to ensure consistent safety state
    for (int i = 0; i < 50; i++) {
        environmentSystem.update(state, 0.016f);
        EXPECT_TRUE(state.launchTubeIntegrity);
        EXPECT_TRUE(state.launchConditionsFavorable);
    }
    
    // Note: Current implementation always maintains safe conditions
    // Future versions could implement dynamic safety zone checking
}
