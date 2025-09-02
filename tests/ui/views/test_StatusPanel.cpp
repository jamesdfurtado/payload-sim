#include <gtest/gtest.h>
#include "ui/views/StatusPanel.h"
#include "sim/SimulationEngine.h"

class StatusPanelTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<StatusPanel> statusPanel;
    
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        statusPanel = std::make_unique<StatusPanel>(*engine);
        statusPanel->setBounds({10, 10, 600, 120});
    }
};

TEST_F(StatusPanelTest, IndicatorStateUpdatesFromSimulationState) {
    auto& state = engine->getState();
    
    state.canLaunchAuthorized = true;
    state.targetValidated = false;
    state.targetAcquired = true;
    state.depthClearanceMet = false;
    state.launchTubeIntegrity = true;
    state.payloadSystemOperational = false;
    state.powerSupplyStable = true;
    state.noFriendlyUnitsInBlastRadius = false;
    state.launchConditionsFavorable = true;
    
    statusPanel->draw();
    
    EXPECT_TRUE(statusPanel->getIndicatorState(0));
    EXPECT_FALSE(statusPanel->getIndicatorState(1));
    EXPECT_TRUE(statusPanel->getIndicatorState(2));
    EXPECT_FALSE(statusPanel->getIndicatorState(3));
    EXPECT_TRUE(statusPanel->getIndicatorState(4));
    EXPECT_FALSE(statusPanel->getIndicatorState(5));
    EXPECT_TRUE(statusPanel->getIndicatorState(6));
    EXPECT_FALSE(statusPanel->getIndicatorState(7));
    EXPECT_TRUE(statusPanel->getIndicatorState(8));
}

TEST_F(StatusPanelTest, IndicatorVisualStateChanges) {
    auto& state = engine->getState();
    
    state.canLaunchAuthorized = false;
    statusPanel->draw();
    EXPECT_FALSE(statusPanel->getIndicatorState(0));
    EXPECT_EQ(statusPanel->getIndicatorColor(0).r, RED.r);
    
    state.canLaunchAuthorized = true;
    statusPanel->draw();
    EXPECT_TRUE(statusPanel->getIndicatorState(0));
    EXPECT_EQ(statusPanel->getIndicatorColor(0).r, GREEN.r);
}

TEST_F(StatusPanelTest, ImmediateRenderingUpdateAfterStateChange) {
    auto& state = engine->getState();
    
    state.targetValidated = false;
    statusPanel->draw();
    EXPECT_FALSE(statusPanel->getIndicatorState(1));
    
    state.targetValidated = true;
    statusPanel->draw();
    EXPECT_TRUE(statusPanel->getIndicatorState(1));
    
    state.targetValidated = false;
    statusPanel->draw();
    EXPECT_FALSE(statusPanel->getIndicatorState(1));
}

TEST_F(StatusPanelTest, MultipleIndicatorsUpdateSimultaneously) {
    auto& state = engine->getState();
    
    state.canLaunchAuthorized = true;
    state.targetValidated = true;
    state.targetAcquired = true;
    state.depthClearanceMet = true;
    state.launchTubeIntegrity = true;
    state.payloadSystemOperational = true;
    state.powerSupplyStable = true;
    state.noFriendlyUnitsInBlastRadius = true;
    state.launchConditionsFavorable = true;
    
    statusPanel->draw();
    
    for (int i = 0; i < 9; i++) {
        EXPECT_TRUE(statusPanel->getIndicatorState(i));
        EXPECT_EQ(statusPanel->getIndicatorColor(i).r, GREEN.r);
    }
    
    state.canLaunchAuthorized = false;
    state.targetValidated = false;
    state.targetAcquired = false;
    state.depthClearanceMet = false;
    state.launchTubeIntegrity = false;
    state.payloadSystemOperational = false;
    state.powerSupplyStable = false;
    state.noFriendlyUnitsInBlastRadius = false;
    state.launchConditionsFavorable = false;
    
    statusPanel->draw();
    
    for (int i = 0; i < 9; i++) {
        EXPECT_FALSE(statusPanel->getIndicatorState(i));
        EXPECT_EQ(statusPanel->getIndicatorColor(i).r, RED.r);
    }
}
