#include <gtest/gtest.h>
#include "simulation/SimulationEngine.h"
#include "systems/PowerSystem.h"
#include "systems/SafetySystem.h"

class SimulationEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        powerSystem = std::make_shared<PowerSystem>();
        safetySystem = std::make_shared<SafetySystem>();
    }

    std::unique_ptr<SimulationEngine> engine;
    std::shared_ptr<PowerSystem> powerSystem;
    std::shared_ptr<SafetySystem> safetySystem;
};

TEST_F(SimulationEngineTest, InitialStateIsCorrect) {
    const auto& state = engine->getState();
    
    // Test initial state values
    EXPECT_FALSE(state.canLaunchAuthorized);
    EXPECT_FALSE(state.targetValidated);
    EXPECT_FALSE(state.targetAcquired);
    EXPECT_FALSE(state.depthClearanceMet);
    EXPECT_TRUE(state.launchTubeIntegrity);
    EXPECT_TRUE(state.payloadSystemOperational);
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    EXPECT_TRUE(state.launchConditionsFavorable);
    
    // Test initial values
    EXPECT_EQ(state.submarineDepthMeters, 100.0f);
    EXPECT_EQ(state.optimalLaunchDepthMeters, 120.0f);
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_EQ(state.submarinePosition.x, 0.0f);
    EXPECT_EQ(state.submarinePosition.y, 0.0f);
}

TEST_F(SimulationEngineTest, CanRegisterSystems) {
    // Should not throw when registering systems
    EXPECT_NO_THROW(engine->registerSystem(powerSystem));
    EXPECT_NO_THROW(engine->registerSystem(safetySystem));
}

TEST_F(SimulationEngineTest, UpdateCallsSystemUpdates) {
    engine->registerSystem(powerSystem);
    engine->registerSystem(safetySystem);
    
    // Should not throw when updating
    EXPECT_NO_THROW(engine->update(0.016f)); // ~60 FPS frame time
    
    // Verify state after systems have processed
    const auto& state = engine->getState();
    
    // Power system should set battery to 100% (temporary implementation)
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_TRUE(state.powerSupplyStable);
    
    // Safety system should start in idle
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    EXPECT_FALSE(state.canLaunchAuthorized);
}
