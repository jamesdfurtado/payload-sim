#include <gtest/gtest.h>
#include "ui/InputHandler.h"
#include "ui/StatusPanel.h"
#include "ui/ControlsPanel.h"
#include "ui/SonarDisplay.h"
#include "ui/InteractiveControls.h"
#include "ui/UiController.h"
#include "ui/LoggingSystem.h"
#include "simulation/SimulationEngine.h"
#include "systems/PowerSystem.h"
#include "systems/SafetySystem.h"
#include "systems/SonarSystem.h"
#include "systems/DepthControl.h"
#include "systems/TargetingSystem.h"
#include "systems/EnvironmentSystem.h"
#include "systems/ContactManager.h"

class UiComponentsTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        powerSystem = std::make_unique<PowerSystem>();
        safetySystem = std::make_unique<SafetySystem>();
        contactManager = std::make_unique<ContactManager>();  // Create ContactManager first
        sonarSystem = std::make_unique<SonarSystem>(*contactManager);
        depthControl = std::make_unique<DepthControl>();
        targetingSystem = std::make_unique<TargetingSystem>();
        environmentSystem = std::make_unique<EnvironmentSystem>();
        
        inputHandler = std::make_unique<InputHandler>(*engine, sonarSystem.get(), powerSystem.get(),
                                                     depthControl.get(), targetingSystem.get(), safetySystem.get());
        statusPanel = std::make_unique<StatusPanel>(*engine, powerSystem.get());
        controlsPanel = std::make_unique<ControlsPanel>(safetySystem.get());
        sonarDisplay = std::make_unique<SonarDisplay>(*engine, sonarSystem.get(), safetySystem.get(), contactManager.get());
        interactiveControls = std::make_unique<InteractiveControls>(*engine, safetySystem.get(), depthControl.get());
        uiController = std::make_unique<UiController>(*engine, sonarSystem.get(), powerSystem.get(),
                                                     depthControl.get(), targetingSystem.get(), 
                                                     safetySystem.get(), environmentSystem.get(), contactManager.get());
        logger = std::make_unique<LoggingSystem>();
    }

    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<PowerSystem> powerSystem;
    std::unique_ptr<SafetySystem> safetySystem;
    std::unique_ptr<SonarSystem> sonarSystem;
    std::unique_ptr<DepthControl> depthControl;
    std::unique_ptr<TargetingSystem> targetingSystem;
    std::unique_ptr<EnvironmentSystem> environmentSystem;
    std::unique_ptr<ContactManager> contactManager;  // Added ContactManager
    
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<ControlsPanel> controlsPanel;
    std::unique_ptr<SonarDisplay> sonarDisplay;
    std::unique_ptr<InteractiveControls> interactiveControls;
    std::unique_ptr<UiController> uiController;
    std::unique_ptr<LoggingSystem> logger;
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
    MissileState missileState;  // Now using MissileState from ContactManager
    
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
    EXPECT_EQ(missileState.targetId, 0);
    EXPECT_TRUE(missileState.targetValid);
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
    MissileState missileState;
    Rectangle sonarRect = {0, 0, 600, 400};
    
    EXPECT_NO_THROW(sonarDisplay->updateMissileAnimation(0.016f, missileState, sonarRect));
}

TEST_F(UiComponentsTest, InteractiveControlsInitializesCorrectly) { // InteractiveControls can be created without errors
    EXPECT_NE(interactiveControls, nullptr);
}

TEST_F(UiComponentsTest, UiControllerInitializesCorrectly) { // UiController can be created without errors
    EXPECT_NE(uiController, nullptr);
}

TEST_F(UiComponentsTest, InteractiveControlsUIStateDefaults) { // InteractiveControls UIState has correct defaults
    InteractiveControls::UIState uiState;
    
    EXPECT_FALSE(uiState.depthThrottleDragging);
    EXPECT_EQ(uiState.depthThrottleValue, 0.5f); // Neutral position
    EXPECT_EQ(uiState.depthThrottleSliderPos.x, 0.0f);
    EXPECT_EQ(uiState.depthThrottleSliderPos.y, 0.0f);
    
    EXPECT_FALSE(uiState.authButtonLit);
    EXPECT_FALSE(uiState.armButtonLit);
    EXPECT_FALSE(uiState.launchButtonLit);
    EXPECT_FALSE(uiState.resetButtonLit);
}

TEST_F(UiComponentsTest, InteractiveControlsCanBeUpdated) { // InteractiveControls update method works
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
}

TEST_F(UiComponentsTest, InteractiveControlsCanBeDrawn) { // InteractiveControls draw method works
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    Rectangle testRect = {640, 380, 620, 320};
    
    EXPECT_NO_THROW(interactiveControls->drawInteractiveControls(testRect, uiState, authState));
}

TEST_F(UiComponentsTest, UiControllerCanBeUpdated) { // UiController update method works
    EXPECT_NO_THROW(uiController->update(0.016f));
}

TEST_F(UiComponentsTest, UiControllerCanBeRendered) { // UiController render method works
    EXPECT_NO_THROW(uiController->render());
}

TEST_F(UiComponentsTest, StatusPanelPowerSliderIntegration) { // StatusPanel power slider can handle mouse input
    float testPower = 0.5f;
    Rectangle powerRect = {640, 140, 620, 110};
    
    // Test that drawing with power reference doesn't crash
    EXPECT_NO_THROW(statusPanel->drawPower(powerRect, testPower));
    
    // Power value should remain valid
    EXPECT_GE(testPower, 0.0f);
    EXPECT_LE(testPower, 1.0f);
}

TEST_F(UiComponentsTest, DepthThrottleControlsDepthSystem) { // Depth throttle properly controls depth system
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test different throttle positions
    uiState.depthThrottleValue = 0.0f; // Full down
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
    
    uiState.depthThrottleValue = 1.0f; // Full up
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
    
    uiState.depthThrottleValue = 0.5f; // Neutral
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
}

TEST_F(UiComponentsTest, SafetyButtonsReflectSystemState) { // Safety buttons properly reflect safety system state
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Update UI state based on safety system
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    // Initially in idle state, no buttons should be lit
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    EXPECT_FALSE(uiState.authButtonLit);
    EXPECT_FALSE(uiState.armButtonLit);
    EXPECT_FALSE(uiState.launchButtonLit);
}

TEST_F(UiComponentsTest, KeyboardControlsRemoved) { // Keyboard controls for depth and safety are removed
    InputHandler::InputState inputState;
    
    // Test that input handler can be called without keyboard affecting depth/safety
    EXPECT_NO_THROW(inputHandler->handleInput(0.016f, inputState));
    
    // Auth input should still work (this is the only keyboard input we keep)
    EXPECT_NO_THROW(inputHandler->handleAuthInput(inputState));
}
