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


TEST_F(PowerSystemTest, InitialWeaponsPowerIsCorrect) { // Initial weapons power is 50%
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.5f);
}

TEST_F(PowerSystemTest, CanRoutePowerToWeapons) { // Can route power to weapons
    powerSystem->routePowerToWeapons(0.8f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.8f);
    
    powerSystem->routePowerToWeapons(0.2f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.2f);
}

TEST_F(PowerSystemTest, ClampsPowerValues) { // Power values are clamped to 0-1 range
    powerSystem->routePowerToWeapons(1.5f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 1.0f);
    
    powerSystem->routePowerToWeapons(-0.5f);
    EXPECT_EQ(powerSystem->getWeaponsPower(), 0.0f);
}

TEST_F(PowerSystemTest, UpdateSetsBatteryTo100Percent) { // Temporary implementation sets battery to 100%
    state.batteryPercent = 25.0f;
    state.powerSupplyStable = false;
    
    powerSystem->update(state, 0.016f);
    
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_TRUE(state.powerSupplyStable);
}

TEST_F(PowerSystemTest, DISABLED_FuturePowerConsumptionTest) { // Future test for power consumption
    state.batteryPercent = 100.0f;
    powerSystem->routePowerToWeapons(1.0f);
    
    for (int i = 0; i < 100; ++i) {
        powerSystem->update(state, 0.1f);
    }
    
    EXPECT_LT(state.batteryPercent, 100.0f);
    
    if (state.batteryPercent < 20.0f) {
        EXPECT_FALSE(state.powerSupplyStable);
    }
}
