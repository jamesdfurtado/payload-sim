#include <gtest/gtest.h>
#include "ui/views/PowerView.h"
#include "sim/SimulationEngine.h"
#include "sim/systems/PowerSystem.h"

class MockPowerSystem : public PowerSystem {
public:
    MockPowerSystem() : PowerSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
    
    float powerLevel = 0.0f;
    float batteryLevel = 50.0f;
    bool powerState = false;
    
    float getPowerLevel() const override { return powerLevel; }
    float getBatteryLevel() const override { return batteryLevel; }
    void setPowerState(bool state) override { powerState = state; }
    bool getPowerState() const { return powerState; }
};

class PowerViewTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<MockPowerSystem> powerSystem;
    std::unique_ptr<PowerView> powerView;
    
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        powerSystem = std::make_unique<MockPowerSystem>();
        powerView = std::make_unique<PowerView>(*engine, *powerSystem);
        powerView->setBounds({10, 10, 300, 100});
    }
};

TEST_F(PowerViewTest, PowerSystemAndSwitchSynchronization) {
    std::vector<float> powerLevels = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    std::vector<bool> expectedStates = {false, false, true, true, true};
    
    for (size_t i = 0; i < powerLevels.size(); i++) {
        powerSystem->powerLevel = powerLevels[i];
        powerView->update(0.016f);
        EXPECT_EQ(powerView->getWeaponsSwitch()->getState(), expectedStates[i]);
    }
}

TEST_F(PowerViewTest, SwitchToggleBidirectionalSync) {
    Vector2 switchPos = {200, 45};
    
    EXPECT_FALSE(powerSystem->getPowerState());
    
    powerView->onMouseDown(switchPos);
    powerView->onMouseUp(switchPos);
    EXPECT_TRUE(powerSystem->getPowerState());
    
    powerView->onMouseDown(switchPos);
    powerView->onMouseUp(switchPos);
    EXPECT_FALSE(powerSystem->getPowerState());
}

TEST_F(PowerViewTest, MouseInteractionHandling) {
    Vector2 switchPos = {200, 45};
    Vector2 outsidePos = {50, 50};
    
    EXPECT_TRUE(powerView->onMouseDown(switchPos));
    EXPECT_TRUE(powerView->onMouseUp(switchPos));
    EXPECT_FALSE(powerView->onMouseDown(outsidePos));
    EXPECT_FALSE(powerView->onMouseUp(outsidePos));
}

TEST_F(PowerViewTest, BatteryDisplayFormatting) {
    powerSystem->batteryLevel = 75.0f;
    powerView->draw();
    
    EXPECT_EQ(powerView->getLastDrawnBatteryText(), "Battery: 75%");
    EXPECT_EQ(powerView->getLastDrawnBatteryColor().r, RAYWHITE.r);
}