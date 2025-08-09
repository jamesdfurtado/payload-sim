#include <gtest/gtest.h>
#include "systems/PowerSystem.h"

class PowerSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        powerSystem = std::make_unique<PowerSystem>();
        
        // Initialize state
        state.batteryPercent = 50.0f;
        state.powerSupplyStable = false;
    }

    std::unique_ptr<PowerSystem> powerSystem;
    SimulationState state;
};

TEST_F(PowerSystemTest, HasCorrectName) {
    EXPECT_STREQ(powerSystem->getName(), "PowerSystem");
}

TEST_F(PowerSystemTest, InitialWeaponsPowerIsCorrect) {
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.5f);
}

TEST_F(PowerSystemTest, CanRoutePowerToWeapons) {
    powerSystem->routePowerToWeapons(0.8f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.8f);
    
    powerSystem->routePowerToWeapons(0.2f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.2f);
}

TEST_F(PowerSystemTest, ClampsPowerValues) {
    // Test upper bound
    powerSystem->routePowerToWeapons(1.5f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 1.0f);
    
    // Test lower bound
    powerSystem->routePowerToWeapons(-0.5f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.0f);
}

TEST_F(PowerSystemTest, UpdateSetsBatteryTo100Percent) {
    // Current implementation is temporary - should set battery to 100%
    state.batteryPercent = 25.0f;
    state.powerSupplyStable = false;
    
    powerSystem->update(state, 0.016f);
    
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_TRUE(state.powerSupplyStable);
}

// Test for future implementation when power consumption is added
TEST_F(PowerSystemTest, DISABLED_FuturePowerConsumptionTest) {
    // This test is disabled until power consumption is implemented
    // When implemented, this should test:
    // - Battery drain over time
    // - Power consumption based on weapons power allocation
    // - Power stability based on battery level and load
    
    state.batteryPercent = 100.0f;
    powerSystem->routePowerToWeapons(1.0f); // Maximum weapons power
    
    // Simulate several seconds of operation
    for (int i = 0; i < 100; ++i) {
        powerSystem->update(state, 0.1f);
    }
    
    // Battery should have drained
    EXPECT_LT(state.batteryPercent, 100.0f);
    
    // Power stability should depend on battery level
    if (state.batteryPercent < 20.0f) {
        EXPECT_FALSE(state.powerSupplyStable);
    }
}
