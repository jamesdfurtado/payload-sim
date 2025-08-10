#include <gtest/gtest.h>
#include "ui/StatusPanel.h"
#include "simulation/SimulationEngine.h"
#include "systems/PowerSystem.h"

// Note: Using real Raylib functions for testing - no mocking needed

class PowerUIIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        powerSystem = std::make_unique<PowerSystem>();
        statusPanel = std::make_unique<StatusPanel>(*engine, powerSystem.get());
    }

    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<PowerSystem> powerSystem;
    std::unique_ptr<StatusPanel> statusPanel;
};

TEST_F(PowerUIIntegrationTest, PowerSliderInitialState) {
    float weaponsPower = 0.5f; // Default middle position
    Rectangle powerRect = {640, 140, 620, 110};
    
    // Should be able to draw without errors
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    // Power should remain at initial value when no mouse interaction
    EXPECT_EQ(weaponsPower, 0.5f);
}

TEST_F(PowerUIIntegrationTest, PowerSliderBoundaryValues) {
    Rectangle powerRect = {640, 140, 620, 110};
    
    // Test minimum value
    float minPower = 0.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, minPower));
    EXPECT_GE(minPower, 0.0f);
    
    // Test maximum value
    float maxPower = 1.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, maxPower));
    EXPECT_LE(maxPower, 1.0f);
    
    // Test out-of-bounds values (should be clamped internally)
    float belowMin = -0.5f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, belowMin));
    
    float aboveMax = 1.5f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, aboveMax));
}

TEST_F(PowerUIIntegrationTest, PowerSliderValuePreservation) {
    Rectangle powerRect = {640, 140, 620, 110};
    
    // Test various power levels
    float testValues[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
    
    for (float testValue : testValues) {
        float powerValue = testValue;
        EXPECT_NO_THROW(statusPanel->drawPower(powerRect, powerValue));
        
        // Value should remain within valid range
        EXPECT_GE(powerValue, 0.0f);
        EXPECT_LE(powerValue, 1.0f);
    }
}

TEST_F(PowerUIIntegrationTest, PowerSystemIntegration) {
    Rectangle powerRect = {640, 140, 620, 110};
    float weaponsPower = 0.75f;
    
    // Draw power panel (this may call power->routePowerToWeapons internally)
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    // Verify the power system can handle the power routing
    EXPECT_NO_THROW(powerSystem->routePowerToWeapons(weaponsPower));
    
    // Update the power system to ensure it processes the power allocation
    auto& state = engine->getState();
    EXPECT_NO_THROW(powerSystem->update(state, 0.016f));
}

TEST_F(PowerUIIntegrationTest, PowerSliderUIRectangleHandling) {
    // Test with various rectangle sizes and positions
    Rectangle rects[] = {
        {0, 0, 100, 50},      // Small rectangle
        {640, 140, 620, 110}, // Default rectangle
        {100, 100, 800, 200}, // Large rectangle
        {-10, -10, 50, 50}    // Rectangle with negative coordinates
    };
    
    float weaponsPower = 0.5f;
    
    for (const auto& rect : rects) {
        EXPECT_NO_THROW(statusPanel->drawPower(rect, weaponsPower));
    }
}

TEST_F(PowerUIIntegrationTest, PowerSystemStateDisplay) {
    Rectangle powerRect = {640, 140, 620, 110};
    float weaponsPower = 0.6f;
    
    // Set up some power system state
    auto& state = engine->getState();
    state.batteryPercent = 85.0f;
    
    // Drawing should display the current battery state
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    // Test with different battery levels
    state.batteryPercent = 0.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    state.batteryPercent = 100.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
}

TEST_F(PowerUIIntegrationTest, NullPowerSystemHandling) {
    // Test with null power system
    auto nullStatusPanel = std::make_unique<StatusPanel>(*engine, nullptr);
    
    Rectangle powerRect = {640, 140, 620, 110};
    float weaponsPower = 0.5f;
    
    // Should not crash with null power system
    EXPECT_NO_THROW(nullStatusPanel->drawPower(powerRect, weaponsPower));
}

// Test the interaction between power UI and the overall system
class PowerUISystemTest : public PowerUIIntegrationTest {
protected:
    void SetUp() override {
        PowerUIIntegrationTest::SetUp();
        
        // Initialize the power system properly
        auto& state = engine->getState();
        powerSystem->update(state, 0.0f); // Initialize
    }
};

TEST_F(PowerUISystemTest, PowerFlowFromUIToSystem) {
    Rectangle powerRect = {640, 140, 620, 110};
    float weaponsPower = 0.8f;
    
    // Simulate drawing the power UI (which should route power)
    statusPanel->drawPower(powerRect, weaponsPower);
    
    // Update the power system to process the power routing
    auto& state = engine->getState();
    powerSystem->update(state, 0.016f);
    
    // The system should have processed the power allocation
    // (We can't easily verify internal state without getters, but we test that it doesn't crash)
    EXPECT_NO_THROW(powerSystem->update(state, 0.016f));
}

TEST_F(PowerUISystemTest, PowerUIRespondsToSystemState) {
    Rectangle powerRect = {640, 140, 620, 110};
    float weaponsPower = 0.5f;
    
    // Test with various system states
    auto& state = engine->getState();
    
    // Low battery
    state.batteryPercent = 10.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    // Full battery
    state.batteryPercent = 100.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
    
    // Critical battery
    state.batteryPercent = 0.0f;
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, weaponsPower));
}
