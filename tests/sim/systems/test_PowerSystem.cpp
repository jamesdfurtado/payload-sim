#include <gtest/gtest.h>
#include "sim/systems/PowerSystem.h"
#include "sim/SimulationState.h"

class PowerSystemTest : public ::testing::Test {
protected:
    PowerSystem powerSystem;
    SimulationState state;
    
    void SetUp() override {
        state = {};
    }
};

TEST_F(PowerSystemTest, InitializesWithFullBatteryAndPowerOff) {
    EXPECT_EQ(powerSystem.getBatteryLevel(), 100.0f);
    EXPECT_EQ(powerSystem.getPowerLevel(), 0.0f);
}

TEST_F(PowerSystemTest, DrainsBatteryWhenPowerOn) {
    powerSystem.setPowerState(true);
    float initialBattery = powerSystem.getBatteryLevel();
    
    powerSystem.update(state, 1.0f);
    
    EXPECT_LT(powerSystem.getBatteryLevel(), initialBattery);
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_GT(state.powerLevel, 0.0f);
}

TEST_F(PowerSystemTest, ChargesBatteryWhenPowerOff) {
    powerSystem.setPowerState(true);
    powerSystem.update(state, 5.0f);
    float drainedBattery = powerSystem.getBatteryLevel();
    
    powerSystem.setPowerState(false);
    powerSystem.update(state, 1.0f);
    
    EXPECT_GT(powerSystem.getBatteryLevel(), drainedBattery);
    EXPECT_FALSE(state.powerSupplyStable);
    EXPECT_EQ(state.powerLevel, drainedBattery / 100.0f);
}

TEST_F(PowerSystemTest, PowerSupplyBecomesUnstableWhenBatteryDepleted) {
    powerSystem.setPowerState(true);
    
    for (int i = 0; i < 30; i++) {
        powerSystem.update(state, 1.0f);
    }
    
    EXPECT_EQ(powerSystem.getBatteryLevel(), 0.0f);
    EXPECT_FALSE(state.powerSupplyStable);
    EXPECT_EQ(state.powerLevel, 0.0f);
}

TEST_F(PowerSystemTest, BatteryDoesNotOvercharge) {
    powerSystem.setPowerState(false);
    
    for (int i = 0; i < 20; i++) {
        powerSystem.update(state, 1.0f);
    }
    
    EXPECT_EQ(powerSystem.getBatteryLevel(), 100.0f);
    EXPECT_EQ(state.powerLevel, 1.0f);
}

TEST_F(PowerSystemTest, BatteryDoesNotGoBelowZero) {
    powerSystem.setPowerState(true);
    
    for (int i = 0; i < 50; i++) {
        powerSystem.update(state, 1.0f);
    }
    
    EXPECT_EQ(powerSystem.getBatteryLevel(), 0.0f);
}

TEST_F(PowerSystemTest, SetPowerLevelControlsDrainRate) {
    powerSystem.setPowerLevel(0.3f);
    powerSystem.update(state, 1.0f);
    
    EXPECT_FALSE(state.powerSupplyStable);
    
    powerSystem.setPowerLevel(0.7f);
    powerSystem.update(state, 1.0f);
    
    EXPECT_TRUE(state.powerSupplyStable);
}

TEST_F(PowerSystemTest, PowerLevelReflectsBatteryPercentage) {
    powerSystem.setPowerState(true);
    powerSystem.update(state, 10.0f);
    
    float expectedPowerLevel = powerSystem.getBatteryLevel() / 100.0f;
    EXPECT_FLOAT_EQ(state.powerLevel, expectedPowerLevel);
}
