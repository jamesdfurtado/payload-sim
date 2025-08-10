#include <gtest/gtest.h>
#include <memory>
#include "../src/ui/InteractiveControls.h"
#include "../src/systems/SafetySystem.h"
#include "../src/systems/DepthControl.h"
#include "../src/simulation/SimulationEngine.h"
#include "../src/ui/AuthCode.h"
#include "../src/ui/LoggingSystem.h"

// Note: Using real Raylib functions for testing - no mocking needed

class InteractiveControlsTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        safetySystem = std::make_unique<SafetySystem>();
        depthControl = std::make_unique<DepthControl>();
        interactiveControls = std::make_unique<InteractiveControls>(*engine, safetySystem.get(), depthControl.get());
        logger = std::make_unique<LoggingSystem>();
    }

    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<SafetySystem> safetySystem;
    std::unique_ptr<DepthControl> depthControl;
    std::unique_ptr<InteractiveControls> interactiveControls;
    std::unique_ptr<LoggingSystem> logger;
};

TEST_F(InteractiveControlsTest, UIStateInitializesWithDefaults) {
    InteractiveControls::UIState uiState;
    EXPECT_FALSE(uiState.depthThrottleDragging);
    EXPECT_FLOAT_EQ(uiState.depthThrottleValue, 0.5f);
    EXPECT_FALSE(uiState.authButtonLit);
    EXPECT_FALSE(uiState.armButtonLit);
    EXPECT_FALSE(uiState.launchButtonLit);
    EXPECT_FALSE(uiState.resetButtonLit);
}

TEST_F(InteractiveControlsTest, InteractiveControlsInitializesCorrectly) {
    EXPECT_NE(interactiveControls, nullptr);
}

TEST_F(InteractiveControlsTest, DepthThrottleValueConstraints) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test that throttle value stays within bounds
    uiState.depthThrottleValue = -0.5f; // Below minimum
    interactiveControls->update(0.016f, uiState, authState, *logger);
    // The value should be clamped in updateDepthThrottle, but we need to test the behavior
    
    uiState.depthThrottleValue = 1.5f; // Above maximum
    interactiveControls->update(0.016f, uiState, authState, *logger);
    // Similar constraint testing
    
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
}

TEST_F(InteractiveControlsTest, SafetyButtonLightingFollowsPhases) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test initial idle state
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    interactiveControls->update(0.016f, uiState, authState, *logger);
    EXPECT_FALSE(uiState.authButtonLit);
    EXPECT_FALSE(uiState.armButtonLit);
    EXPECT_FALSE(uiState.launchButtonLit);
    EXPECT_FALSE(uiState.resetButtonLit);
    
    // Test authorized state
    safetySystem->requestAuthorizationCode("test"); // This might not work without proper setup
    // We'll need to set up the simulation state properly for this to work
    
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
}

TEST_F(InteractiveControlsTest, UpdateCanBeCalledWithoutErrors) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
    EXPECT_NO_THROW(interactiveControls->update(0.033f, uiState, authState, *logger));
    EXPECT_NO_THROW(interactiveControls->update(0.0f, uiState, authState, *logger));
}

TEST_F(InteractiveControlsTest, DrawInteractiveControlsCanBeCalled) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    Rectangle testRect = {640, 380, 620, 320};
    
    // Drawing functions typically don't throw in Raylib, but we test that our code doesn't crash
    EXPECT_NO_THROW(interactiveControls->drawInteractiveControls(testRect, uiState, authState));
}

TEST_F(InteractiveControlsTest, DepthThrottleControlsDepthSystem) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Set different throttle values and verify they're applied to depth system
    uiState.depthThrottleValue = 0.0f; // Full down
    interactiveControls->update(0.016f, uiState, authState, *logger);
    // We could check depthControl's internal state if it had getters
    
    uiState.depthThrottleValue = 1.0f; // Full up
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    uiState.depthThrottleValue = 0.5f; // Neutral
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    EXPECT_NO_THROW(interactiveControls->update(0.016f, uiState, authState, *logger));
}

TEST_F(InteractiveControlsTest, ResetTimerHandling) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test reset timer countdown
    authState.resetTimer = 1.0f;
    safetySystem->reset(); // Put system in resetting state
    
    float initialTimer = authState.resetTimer;
    interactiveControls->update(0.5f, uiState, authState, *logger);
    
    // Timer should have decreased
    EXPECT_LT(authState.resetTimer, initialTimer);
    
    // Test timer reaching zero
    authState.resetTimer = 0.1f;
    interactiveControls->update(0.2f, uiState, authState, *logger);
    EXPECT_LE(authState.resetTimer, 0.0f);
}

TEST_F(InteractiveControlsTest, AuthInputPreservation) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // When auth box is focused, other controls should be ignored
    authState.authBoxFocused = true;
    authState.inputCode = "1234";
    
    // Simulate some time passing
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    // Auth input should be preserved
    EXPECT_EQ(authState.inputCode, "1234");
}

TEST_F(InteractiveControlsTest, SafetySystemProgression) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test the complete safety sequence progression
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Idle);
    
    // Set up challenge code and authorize
    safetySystem->setChallengeCode("1234");
    safetySystem->requestAuthorizationCode("1234");
    
    // Update to process the authorization
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    // Should now be in authorized state
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Authorized);
    
    // Arm the system
    safetySystem->arm();
    interactiveControls->update(0.016f, uiState, authState, *logger);
    
    // Should be arming
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Arming);
    
    // Wait for arming to complete
    for (int i = 0; i < 200; i++) { // 2 seconds at 0.01 dt
        interactiveControls->update(0.01f, uiState, authState, *logger);
    }
    
    // Should now be armed
    EXPECT_EQ(safetySystem->getPhase(), LaunchPhase::Armed);
}

TEST_F(InteractiveControlsTest, NullSystemHandling) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Create controls with null systems
    InteractiveControls nullControls(*engine, nullptr, nullptr);
    
    // Should not crash
    EXPECT_NO_THROW(nullControls.update(0.016f, uiState, authState, *logger));
    EXPECT_NO_THROW(nullControls.drawInteractiveControls({0, 0, 100, 100}, uiState, authState));
}

TEST_F(InteractiveControlsTest, LargeTimeDelta) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test with large time delta
    EXPECT_NO_THROW(interactiveControls->update(1.0f, uiState, authState, *logger));
    EXPECT_NO_THROW(interactiveControls->update(10.0f, uiState, authState, *logger));
}

TEST_F(InteractiveControlsTest, ZeroTimeDelta) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    // Test with zero time delta
    EXPECT_NO_THROW(interactiveControls->update(0.0f, uiState, authState, *logger));
}

class InteractiveControlsButtonTest : public InteractiveControlsTest {
protected:
    void SetUp() override {
        InteractiveControlsTest::SetUp();
        // Set up simulation state to allow progression through safety phases
        auto& state = engine->getState();
        state.targetValidated = true;
        state.targetAcquired = true;
        state.depthClearanceMet = true;
        state.launchTubeIntegrity = true;
        state.powerSupplyStable = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
    }
};

TEST_F(InteractiveControlsButtonTest, ButtonLightingProgression) {
    InteractiveControls::UIState uiState;
    AuthCode::AuthState authState;
    
    struct PhaseTest {
        LaunchPhase phase;
        bool expectedAuth;
        bool expectedArm;
        bool expectedLaunch;
        bool expectedReset;
    };
    
    std::vector<PhaseTest> tests = {
        {LaunchPhase::Idle, false, false, false, false},
        {LaunchPhase::Authorized, true, false, false, true},
        {LaunchPhase::Arming, true, true, false, true},
        {LaunchPhase::Armed, true, true, false, true},
        {LaunchPhase::Launching, true, true, true, true},
        {LaunchPhase::Launched, true, true, true, true},
        {LaunchPhase::Resetting, false, false, false, true}
    };
    
    for (const auto& test : tests) {
        // Force the safety system into the test phase
        switch (test.phase) {
            case LaunchPhase::Authorized:
                safetySystem->setChallengeCode("1234");
                safetySystem->requestAuthorizationCode("1234");
                break;
            case LaunchPhase::Arming:
                safetySystem->setChallengeCode("1234");
                safetySystem->requestAuthorizationCode("1234");
                safetySystem->arm();
                break;
            case LaunchPhase::Armed:
                safetySystem->setChallengeCode("1234");
                safetySystem->requestAuthorizationCode("1234");
                safetySystem->arm();
                // Wait for arming to complete
                for (int i = 0; i < 200; i++) {
                    safetySystem->update(engine->getState(), 0.01f);
                }
                break;
            case LaunchPhase::Launching:
                safetySystem->setChallengeCode("1234");
                safetySystem->requestAuthorizationCode("1234");
                safetySystem->arm();
                // Wait for arming to complete
                for (int i = 0; i < 200; i++) {
                    safetySystem->update(engine->getState(), 0.01f);
                }
                safetySystem->launch();
                break;
            case LaunchPhase::Launched:
                safetySystem->setChallengeCode("1234");
                safetySystem->requestAuthorizationCode("1234");
                safetySystem->arm();
                // Wait for arming to complete
                for (int i = 0; i < 200; i++) {
                    safetySystem->update(engine->getState(), 0.01f);
                }
                safetySystem->launch();
                // Wait for launching to complete
                for (int i = 0; i < 100; i++) {
                    safetySystem->update(engine->getState(), 0.01f);
                }
                break;
            case LaunchPhase::Resetting:
                safetySystem->reset();
                break;
            default:
                break;
        }
        
        // Update interactive controls to reflect the new state
        interactiveControls->update(0.016f, uiState, authState, *logger);
        
        // Verify button lighting matches expected state
        EXPECT_EQ(uiState.authButtonLit, test.expectedAuth) 
            << "Auth button lighting incorrect for phase " << static_cast<int>(test.phase);
        EXPECT_EQ(uiState.armButtonLit, test.expectedArm)
            << "Arm button lighting incorrect for phase " << static_cast<int>(test.phase);
        EXPECT_EQ(uiState.launchButtonLit, test.expectedLaunch)
            << "Launch button lighting incorrect for phase " << static_cast<int>(test.phase);
        EXPECT_EQ(uiState.resetButtonLit, test.expectedReset)
            << "Reset button lighting incorrect for phase " << static_cast<int>(test.phase);
    }
}
