#include <gtest/gtest.h>
#include "sim/SimulationEngine.h"
#include "sim/ISystem.h"
#include "sim/SimulationState.h"

class MockSystem : public ISystem {
public:
    MockSystem(const std::string& name) : systemName(name), updateCount(0) {}
    
    const char* getName() const override { return systemName.c_str(); }
    
    void update(SimulationState& state, float dt) override {
        updateCount++;
        lastDt = dt;
        state.targetingStability = 0.5f;
    }
    
    int getUpdateCount() const { return updateCount; }
    float getLastDt() const { return lastDt; }
    
private:
    std::string systemName;
    int updateCount;
    float lastDt = 0.0f;
};

class SimulationEngineTest : public ::testing::Test {
protected:
    SimulationEngine engine;
    
    void SetUp() override {
    }
};

TEST_F(SimulationEngineTest, InitializesWithEmptyState) {
    const auto& state = engine.getState();
    EXPECT_FALSE(state.targetAcquired);
    EXPECT_FALSE(state.targetValidated);
    EXPECT_FALSE(state.missileLaunched);
    EXPECT_FALSE(state.missileActive);
    EXPECT_FALSE(state.powerSupplyStable);
    EXPECT_FALSE(state.payloadSystemOperational);
    EXPECT_FALSE(state.canLaunchAuthorized);
}

TEST_F(SimulationEngineTest, RegistersAndUpdatesSystems) {
    auto mockSystem = std::make_shared<MockSystem>("TestSystem");
    engine.registerSystem(mockSystem);
    
    engine.update(0.016f);
    
    EXPECT_EQ(mockSystem->getUpdateCount(), 1);
    EXPECT_FLOAT_EQ(mockSystem->getLastDt(), 0.016f);
}

TEST_F(SimulationEngineTest, UpdatesMultipleSystemsInOrder) {
    auto system1 = std::make_shared<MockSystem>("System1");
    auto system2 = std::make_shared<MockSystem>("System2");
    auto system3 = std::make_shared<MockSystem>("System3");
    
    engine.registerSystem(system1);
    engine.registerSystem(system2);
    engine.registerSystem(system3);
    
    engine.update(0.032f);
    
    EXPECT_EQ(system1->getUpdateCount(), 1);
    EXPECT_EQ(system2->getUpdateCount(), 1);
    EXPECT_EQ(system3->getUpdateCount(), 1);
    EXPECT_FLOAT_EQ(system1->getLastDt(), 0.032f);
    EXPECT_FLOAT_EQ(system2->getLastDt(), 0.032f);
    EXPECT_FLOAT_EQ(system3->getLastDt(), 0.032f);
}

TEST_F(SimulationEngineTest, SystemsModifySharedState) {
    auto mockSystem = std::make_shared<MockSystem>("TestSystem");
    engine.registerSystem(mockSystem);
    
    EXPECT_NE(engine.getState().targetingStability, 0.5f);
    
    engine.update(0.016f);
    
    EXPECT_FLOAT_EQ(engine.getState().targetingStability, 0.5f);
}

TEST_F(SimulationEngineTest, HandlesMultipleUpdates) {
    auto mockSystem = std::make_shared<MockSystem>("TestSystem");
    engine.registerSystem(mockSystem);
    
    engine.update(0.016f);
    engine.update(0.020f);
    engine.update(0.024f);
    
    EXPECT_EQ(mockSystem->getUpdateCount(), 3);
    EXPECT_FLOAT_EQ(mockSystem->getLastDt(), 0.024f);
}

TEST_F(SimulationEngineTest, ProvidesStateAccess) {
    auto& state = engine.getState();
    state.targetAcquired = true;
    state.powerSupplyStable = true;
    
    const auto& constState = engine.getState();
    EXPECT_TRUE(constState.targetAcquired);
    EXPECT_TRUE(constState.powerSupplyStable);
}

TEST_F(SimulationEngineTest, HandlesEmptySystemsList) {
    engine.update(0.016f);
    
    const auto& state = engine.getState();
    EXPECT_FALSE(state.targetAcquired);
}
