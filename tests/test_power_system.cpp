#include <gtest/gtest.h>
#include "systems/PowerSystem.h"

class PowerSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        powerSystem = std::make_unique<PowerSystem>();
        
        state.batteryPercent = 50.0f;
        state.powerSupplyStable = false;
    }

    std::unique_ptr<PowerSystem> powerSystem;
    SimulationState state;
};

TEST_F(PowerSystemTest, InitialPowerLevelIsCorrect) { // Initial power level is OFF
    EXPECT_EQ(powerSystem->getPowerLevel(), 0.0f);
}

TEST_F(PowerSystemTest, CanSetPowerLevel) { // Can set power level
    powerSystem->setPowerLevel(0.8f);
    EXPECT_EQ(powerSystem->getPowerLevel(), 0.8f);
    
    powerSystem->setPowerLevel(0.2f);
    EXPECT_EQ(powerSystem->getPowerLevel(), 0.2f);
}

TEST_F(PowerSystemTest, ClampsPowerValues) { // Power values are clamped to 0-1 range
    powerSystem->setPowerLevel(1.5f);
    EXPECT_EQ(powerSystem->getPowerLevel(), 1.0f);
    
    powerSystem->setPowerLevel(-0.5f);
    EXPECT_EQ(powerSystem->getPowerLevel(), 0.0f);
}

TEST_F(PowerSystemTest, UpdateHandlesPowerStates) { // Test power state handling
    state.batteryPercent = 25.0f;
    state.powerSupplyStable = false;
    
    powerSystem->setPowerLevel(1.0f); // Turn power ON
    powerSystem->update(state, 0.016f);
    
    EXPECT_GT(state.batteryPercent, 25.0f); // Battery should deplete
    EXPECT_TRUE(state.powerSupplyStable); // Power ON and battery >0% should be stable
}

TEST_F(PowerSystemTest, PowerConsumptionTest) { // Test power consumption and battery behavior
    state.batteryPercent = 100.0f;
    powerSystem->setPowerLevel(1.0f); // Power ON (>=0.5)
    
    // Run for a few seconds - battery should deplete
    for (int i = 0; i < 50; ++i) {
        powerSystem->update(state, 0.1f);
    }
    
    EXPECT_LT(state.batteryPercent, 100.0f);
    EXPECT_TRUE(state.powerSupplyStable); // Power ON and battery >0% should be stable
    
    // Test power OFF mode (<0.5)
    powerSystem->setPowerLevel(0.0f);
    float batteryBefore = state.batteryPercent;
    
    // Run for a few seconds - battery should recharge
    for (int i = 0; i < 50; ++i) {
        powerSystem->update(state, 0.1f);
    }
    
    EXPECT_GT(state.batteryPercent, batteryBefore);
    EXPECT_FALSE(state.powerSupplyStable); // Power OFF should not be stable
    
    // Test power stable when battery is depleted
    powerSystem->setPowerLevel(1.0f); // Turn power back ON
    state.batteryPercent = 0.0f; // Deplete battery
    
    powerSystem->update(state, 0.1f);
    EXPECT_FALSE(state.powerSupplyStable); // Power ON but battery 0% should not be stable
}
