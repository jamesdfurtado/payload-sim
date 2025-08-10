#include <gtest/gtest.h>
#include "ui/InputHandler.h"
#include "ui/StatusPanel.h"
#include "ui/ControlsPanel.h"
#include "ui/SonarDisplay.h"
#include "simulation/SimulationEngine.h"
#include "systems/PowerSystem.h"
#include "systems/SafetySystem.h"
#include "systems/SonarSystem.h"
#include "systems/DepthControl.h"
#include "systems/TargetingSystem.h"

class UiComponentsTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        powerSystem = std::make_unique<PowerSystem>();
        safetySystem = std::make_unique<SafetySystem>();
        sonarSystem = std::make_unique<SonarSystem>();
        depthControl = std::make_unique<DepthControl>();
        targetingSystem = std::make_unique<TargetingSystem>();
        
        inputHandler = std::make_unique<InputHandler>(*engine, sonarSystem.get(), 
                                                     powerSystem.get(), depthControl.get(),
                                                     targetingSystem.get(), safetySystem.get());
        statusPanel = std::make_unique<StatusPanel>(*engine, powerSystem.get());
        controlsPanel = std::make_unique<ControlsPanel>(safetySystem.get());
        sonarDisplay = std::make_unique<SonarDisplay>(*engine, sonarSystem.get(), safetySystem.get());
    }

    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<PowerSystem> powerSystem;
    std::unique_ptr<SafetySystem> safetySystem;
    std::unique_ptr<SonarSystem> sonarSystem;
    std::unique_ptr<DepthControl> depthControl;
    std::unique_ptr<TargetingSystem> targetingSystem;
    
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<ControlsPanel> controlsPanel;
    std::unique_ptr<SonarDisplay> sonarDisplay;
};

TEST_F(UiComponentsTest, InputHandlerInitializesCorrectly) { // InputHandler can be created without errors
    EXPECT_NE(inputHandler, nullptr);
}

TEST_F(UiComponentsTest, StatusPanelInitializesCorrectly) { // StatusPanel can be created without errors
    EXPECT_NE(statusPanel, nullptr);
}

TEST_F(UiComponentsTest, ControlsPanelInitializesCorrectly) { // ControlsPanel can be created without errors
    EXPECT_NE(controlsPanel, nullptr);
}

TEST_F(UiComponentsTest, SonarDisplayInitializesCorrectly) { // SonarDisplay can be created without errors
    EXPECT_NE(sonarDisplay, nullptr);
}

TEST_F(UiComponentsTest, InputStateInitializesWithDefaults) { // InputState has correct default values
    InputHandler::InputState inputState;
    
    EXPECT_FALSE(inputState.authBoxFocused);
    EXPECT_TRUE(inputState.authCodeInput.empty());
    EXPECT_TRUE(inputState.authChallengeCode.empty());
    EXPECT_TRUE(inputState.feedbackMessage.empty());
    EXPECT_EQ(inputState.uiWeaponsPower, 0.5f);
    EXPECT_EQ(inputState.resetTimer, 0.0f);
}

TEST_F(UiComponentsTest, MissileStateInitializesWithDefaults) { // MissileState has correct default values
    SonarDisplay::MissileState missileState;
    
    EXPECT_FALSE(missileState.active);
    EXPECT_EQ(missileState.position.x, 0.0f);
    EXPECT_EQ(missileState.position.y, 0.0f);
    EXPECT_EQ(missileState.velocity.x, 0.0f);
    EXPECT_EQ(missileState.velocity.y, 0.0f);
    EXPECT_EQ(missileState.target.x, 0.0f);
    EXPECT_EQ(missileState.target.y, 0.0f);
    EXPECT_TRUE(missileState.trail.empty());
    EXPECT_EQ(missileState.progress, 0.0f);
    EXPECT_EQ(missileState.explosionTimer, 0.0f);
    EXPECT_EQ(missileState.targetIndex, -1);
}

TEST_F(UiComponentsTest, SonarDisplayCoordinateConversion) { // Coordinate conversion works correctly
    Rectangle testRect = {100, 50, 400, 300};
    Vector2 worldPos = {0, 0}; // Center of world
    
    Vector2 screenPos = sonarDisplay->worldToScreen(worldPos, testRect);
    Vector2 backToWorld = sonarDisplay->screenToWorld(screenPos, testRect);
    
    // Should convert to center of screen rect
    EXPECT_NEAR(screenPos.x, 300.0f, 1.0f); // 100 + 400/2
    EXPECT_NEAR(screenPos.y, 200.0f, 1.0f); // 50 + 300/2
    
    // Should convert back to original world coordinates
    EXPECT_NEAR(backToWorld.x, 0.0f, 1.0f);
    EXPECT_NEAR(backToWorld.y, 0.0f, 1.0f);
}

TEST_F(UiComponentsTest, InputHandlerCanBeUpdated) { // InputHandler update method can be called without errors
    InputHandler::InputState inputState;
    
    EXPECT_NO_THROW(inputHandler->handleInput(0.016f, inputState));
}

TEST_F(UiComponentsTest, SonarDisplayCanBeUpdated) { // SonarDisplay update method can be called without errors
    SonarDisplay::MissileState missileState;
    Rectangle sonarRect = {0, 0, 600, 400};
    
    EXPECT_NO_THROW(sonarDisplay->updateMissileAnimation(0.016f, missileState, sonarRect));
}
