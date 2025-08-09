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

TEST_F(SimulationEngineTest, InitialStateIsCorrect) { // Verify default simulation state values
    const auto& state = engine->getState();
    
    EXPECT_FALSE(state.canLaunchAuthorized);
    EXPECT_FALSE(state.targetValidated);
    EXPECT_FALSE(state.targetAcquired);
    EXPECT_FALSE(state.depthClearanceMet);
    EXPECT_TRUE(state.launchTubeIntegrity);
    EXPECT_TRUE(state.payloadSystemOperational);
    EXPECT_TRUE(state.powerSupplyStable);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    EXPECT_TRUE(state.launchConditionsFavorable);
    
    EXPECT_EQ(state.submarineDepthMeters, 100.0f);
    EXPECT_EQ(state.optimalLaunchDepthMeters, 120.0f);
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_EQ(state.submarinePosition.x, 0.0f);
    EXPECT_EQ(state.submarinePosition.y, 0.0f);
}

TEST_F(SimulationEngineTest, CanRegisterSystems) { // Systems can be registered without errors
    EXPECT_NO_THROW(engine->registerSystem(powerSystem));
    EXPECT_NO_THROW(engine->registerSystem(safetySystem));
}

TEST_F(SimulationEngineTest, UpdateCallsSystemUpdates) { // Engine updates all registered systems correctly
    engine->registerSystem(powerSystem);
    engine->registerSystem(safetySystem);
    
    EXPECT_NO_THROW(engine->update(0.016f));
    
    const auto& state = engine->getState();
    
    EXPECT_EQ(state.batteryPercent, 100.0f);
    EXPECT_TRUE(state.powerSupplyStable);
    
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    EXPECT_FALSE(state.canLaunchAuthorized);
}
