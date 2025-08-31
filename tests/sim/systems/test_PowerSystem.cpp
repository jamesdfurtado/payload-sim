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
    powerSystem.setPowerState(true);
    
    // Drain battery first by running with power on
    for (int i = 0; i < 300; i++) { // 5 seconds of drain
        powerSystem.update(state, 0.016f);
    }
    float drainedBattery = powerSystem.getBatteryLevel();
    EXPECT_LT(drainedBattery, 80.0f); // Should be significantly drained
    
    // Turn off power and charge
    powerSystem.setPowerState(false);
    powerSystem.update(state, 1.0f); // 1 second charging
    
    EXPECT_GT(powerSystem.getBatteryLevel(), drainedBattery);
    EXPECT_GT(powerSystem.getBatteryLevel(), drainedBattery + 10.0f); // Should gain significant charge
}

TEST_F(PowerSystemTest, SimulationStateUpdate) {
    powerSystem.setPowerState(true);
    powerSystem.update(state, 0.1f);
    
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_GT(state.powerLevel, 0.0f);
}

TEST_F(PowerSystemTest, PowerLossOnBatteryDepletion) {
    powerSystem.setPowerState(true);
    
    // Drain battery completely
    for (int i = 0; i < 1600; i++) { // Should drain battery completely
        powerSystem.update(state, 0.016f);
        
        if (powerSystem.getBatteryLevel() <= 0.0f) {
            break;
        }
    }
    
    EXPECT_FLOAT_EQ(powerSystem.getBatteryLevel(), 0.0f);
    EXPECT_FALSE(state.powerSupplyStable); // Power should be unstable with dead battery
}

TEST_F(PowerSystemTest, LaunchSequencePowerRequirement) {
    // Simulate launch sequence power requirements
    powerSystem.setPowerState(true);
    
    // Verify stable power for launch
    powerSystem.update(state, 0.016f);
    EXPECT_TRUE(state.powerSupplyStable);
    
    // Simulate extended operation during launch sequence
    for (int i = 0; i < 600; i++) { // 10 seconds of operation
        powerSystem.update(state, 0.016f);
    }
    
    // Should still have power for launch (battery lasts ~25 seconds)
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_GT(powerSystem.getBatteryLevel(), 50.0f);
}

TEST_F(PowerSystemTest, EmergencyPowerScenario) {
    powerSystem.setPowerState(true);
    
    // Drain to critical level (simulate emergency)
    for (int i = 0; i < 1500; i++) {
        powerSystem.update(state, 0.016f);
        if (powerSystem.getBatteryLevel() < 10.0f) {
            break;
        }
    }
    
    // Should still be operational at low battery
    if (powerSystem.getBatteryLevel() > 0.0f) {
        EXPECT_TRUE(state.powerSupplyStable);
        EXPECT_GT(state.powerLevel, 0.0f);
    }
    
    // Complete depletion should cut power
    for (int i = 0; i < 200; i++) {
        powerSystem.update(state, 0.016f);
    }
    
    EXPECT_FALSE(state.powerSupplyStable);
}
