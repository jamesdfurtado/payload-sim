#include <gtest/gtest.h>
#include "sim/SimulationEngine.h"
#include "sim/SimulationState.h"

class SimulationEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
    }
    
    void TearDown() override {
        // Clean up after tests
    }
};

TEST_F(SimulationEngineTest, InitializesCorrectly) {
    SimulationEngine engine;
    
    // Engine should initialize with default state
    const auto& state = engine.getState();
    EXPECT_FALSE(state.targetValidated);
    EXPECT_FALSE(state.targetAcquired);
    EXPECT_FALSE(state.depthClearanceMet);
    EXPECT_TRUE(state.launchTubeIntegrity);
    EXPECT_FALSE(state.payloadSystemOperational);
    EXPECT_FALSE(state.powerSupplyStable);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    EXPECT_TRUE(state.launchConditionsFavorable);
    EXPECT_FALSE(state.canLaunchAuthorized);
    EXPECT_FALSE(state.missileLaunched);
    EXPECT_FALSE(state.missileActive);
    EXPECT_FALSE(state.explosionActive);
    EXPECT_EQ(state.missileTargetId, 0);
    EXPECT_FLOAT_EQ(state.explosionTimer, 0.0f);
}

TEST_F(SimulationEngineTest, UpdatesState) {
    SimulationEngine engine;
    
    // Create a mock system to test update behavior
    class MockUpdateSystem : public ISystem {
    public:
        const char* getName() const override { return "MockUpdateSystem"; }
        void update(SimulationState& state, float dt) override {
            updateCalled = true;
            lastDt = dt;
            state.targetValidated = true; // Modify state to verify update happened
        }
        bool updateCalled = false;
        float lastDt = 0.0f;
    };
    
    auto mockSystem = std::make_shared<MockUpdateSystem>();
    engine.registerSystem(mockSystem);
    
    // Update engine
    engine.update(0.016f);
    
    // Verify mock system was called
    EXPECT_TRUE(mockSystem->updateCalled);
    EXPECT_FLOAT_EQ(mockSystem->lastDt, 0.016f);
    
    // Verify state was modified
    EXPECT_TRUE(engine.getState().targetValidated);
}

TEST_F(SimulationEngineTest, HandlesSystemsCorrectly) {
    SimulationEngine engine;
    
    // Create multiple mock systems
    class CountingSystem : public ISystem {
    public:
        explicit CountingSystem(int id) : systemId(id) {}
        const char* getName() const override { return "CountingSystem"; }
        void update(SimulationState& state, float dt) override {
            updateCount++;
            // Each system modifies a different part of state
            if (systemId == 1) state.targetValidated = true;
            if (systemId == 2) state.targetAcquired = true;
        }
        int updateCount = 0;
        int systemId;
    };
    
    auto system1 = std::make_shared<CountingSystem>(1);
    auto system2 = std::make_shared<CountingSystem>(2);
    
    // Register multiple systems
    engine.registerSystem(system1);
    engine.registerSystem(system2);
    
    // Update engine
    engine.update(0.016f);
    
    // Both systems should have been updated
    EXPECT_EQ(system1->updateCount, 1);
    EXPECT_EQ(system2->updateCount, 1);
    
    // Both should have modified state
    const auto& state = engine.getState();
    EXPECT_TRUE(state.targetValidated);
    EXPECT_TRUE(state.targetAcquired);
    
    // Update again
    engine.update(0.016f);
    
    // Count should increment
    EXPECT_EQ(system1->updateCount, 2);
    EXPECT_EQ(system2->updateCount, 2);
}
