#include <gtest/gtest.h>
#include "sim/ISystem.h"
#include "sim/SimulationState.h"

// Mock system for testing ISystem interface
class MockSystem : public ISystem {
public:
    const char* getName() const override { return "MockSystem"; }
    void update(SimulationState& state, float dt) override {
        updateCalled = true;
        lastDt = dt;
    }
    
    bool updateCalled = false;
    float lastDt = 0.0f;
};

class ISystemTest : public ::testing::Test {
protected:
    MockSystem mockSystem;
    SimulationState state;
};

TEST_F(ISystemTest, InterfaceBasics) {
    EXPECT_STREQ(mockSystem.getName(), "MockSystem");
    EXPECT_FALSE(mockSystem.updateCalled);
}

TEST_F(ISystemTest, UpdateInterface) {
    mockSystem.update(state, 0.016f);
    EXPECT_TRUE(mockSystem.updateCalled);
    EXPECT_FLOAT_EQ(mockSystem.lastDt, 0.016f);
}
