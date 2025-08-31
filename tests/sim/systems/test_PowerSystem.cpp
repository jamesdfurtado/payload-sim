#include <gtest/gtest.h>
#include "sim/systems/PowerSystem.h"
#include "sim/SimulationState.h"

class PowerSystemTest : public ::testing::Test {
protected:
    PowerSystem powerSystem;
    SimulationState state;
    
    void SetUp() override {
        // Initialize clean state
    }
};

TEST_F(PowerSystemTest, DefaultState) {
    EXPECT_FLOAT_EQ(powerSystem.getPowerLevel(), 0.0f);
    EXPECT_FLOAT_EQ(powerSystem.getBatteryLevel(), 100.0f);
}

TEST_F(PowerSystemTest, PowerStateToggle) {
    powerSystem.setPowerState(true);
    EXPECT_FLOAT_EQ(powerSystem.getPowerLevel(), 1.0f);
    
    powerSystem.setPowerState(false);
    EXPECT_FLOAT_EQ(powerSystem.getPowerLevel(), 0.0f);
}

TEST_F(PowerSystemTest, BatteryDrain) {
    powerSystem.setPowerState(true);
    float initialBattery = powerSystem.getBatteryLevel();
    
    powerSystem.update(state, 1.0f); // 1 second
    
    EXPECT_LT(powerSystem.getBatteryLevel(), initialBattery);
}

TEST_F(PowerSystemTest, BatteryCharge) {
    powerSystem.setPowerState(false);
    powerSystem.setPowerLevel(0.0f); // Ensure off
    
    // Drain battery first
    powerSystem.update(state, -10.0f); // Simulate drained battery
    float drainedBattery = powerSystem.getBatteryLevel();
    
    powerSystem.update(state, 1.0f); // 1 second charging
    
    EXPECT_GT(powerSystem.getBatteryLevel(), drainedBattery);
}

TEST_F(PowerSystemTest, SimulationStateUpdate) {
    powerSystem.setPowerState(true);
    powerSystem.update(state, 0.1f);
    
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_GT(state.powerLevel, 0.0f);
}
