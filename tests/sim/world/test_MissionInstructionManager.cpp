#include <gtest/gtest.h>
#include "sim/world/MissionInstructionManager.h"
#include "sim/SimulationEngine.h"
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"

class MissionInstructionManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<LaunchSequenceHandler> launchHandler;
    std::unique_ptr<MissionInstructionManager> missionManager;
    
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        launchHandler = std::make_unique<LaunchSequenceHandler>(*engine);
        missionManager = std::make_unique<MissionInstructionManager>(*engine, launchHandler.get());
        
        auto& state = engine->getState();
        state.depthClearanceMet = false;
        state.targetAcquired = false;
        state.powerSupplyStable = false;
    }
    
    void setupValidConditions() {
        auto& state = engine->getState();
        state.depthClearanceMet = true;
        state.targetAcquired = true;
        state.powerSupplyStable = true;
        state.targetValidated = true;
        state.launchTubeIntegrity = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
    }
};

TEST_F(MissionInstructionManagerTest, StartsWithDepthAdjustment) {
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::ADJUST_DEPTH);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::DEPTH_THROTTLE);
    EXPECT_FALSE(instruction.isComplete);
    EXPECT_FALSE(instruction.instructionText.empty());
}

TEST_F(MissionInstructionManagerTest, ProgressesToTargetAcquisition) {
    auto& state = engine->getState();
    state.depthClearanceMet = true;
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::ACQUIRE_TARGET);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::SONAR_BOX);
    EXPECT_FALSE(instruction.isComplete);
}

TEST_F(MissionInstructionManagerTest, ProgressesToPowerActivation) {
    auto& state = engine->getState();
    state.depthClearanceMet = true;
    state.targetAcquired = true;
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::TURN_ON_POWER);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::POWER_SWITCH);
    EXPECT_FALSE(instruction.isComplete);
}

TEST_F(MissionInstructionManagerTest, ProgressesToAuthorization) {
    setupValidConditions();
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::CLICK_AUTHORIZE);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::AUTHORIZE_BUTTON);
    EXPECT_FALSE(instruction.isComplete);
}

TEST_F(MissionInstructionManagerTest, ShowsAuthCodeInputWhenPending) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::INPUT_AUTH_CODE);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::KEYPAD_AREA);
    EXPECT_FALSE(instruction.isComplete);
    EXPECT_NE(instruction.instructionText.find(launchHandler->getAuthCode()), std::string::npos);
}

TEST_F(MissionInstructionManagerTest, ProgressesToArmingAfterAuthorization) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::CLICK_ARM);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::ARM_BUTTON);
    EXPECT_FALSE(instruction.isComplete);
}

TEST_F(MissionInstructionManagerTest, ShowsArmingInProgressMessage) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    launchHandler->requestArm();
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::CLICK_ARM);
    EXPECT_NE(instruction.instructionText.find("Arming"), std::string::npos);
}

TEST_F(MissionInstructionManagerTest, ProgressesToLaunchWhenArmed) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    launchHandler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::CLICK_LAUNCH);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::LAUNCH_BUTTON);
    EXPECT_FALSE(instruction.isComplete);
}

TEST_F(MissionInstructionManagerTest, ShowsLaunchingInProgressMessage) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    launchHandler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    launchHandler->requestLaunch();
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::CLICK_LAUNCH);
    EXPECT_NE(instruction.instructionText.find("Launching"), std::string::npos);
}

TEST_F(MissionInstructionManagerTest, ShowsMissionCompleteWhenLaunched) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    launchHandler->requestArm();
    
    for (int i = 0; i < 150; i++) {
        engine->update(0.016f);
    }
    
    launchHandler->requestLaunch();
    
    for (int i = 0; i < 70; i++) {
        engine->update(0.016f);
    }
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::MISSION_COMPLETE);
    EXPECT_EQ(instruction.pulsateTarget, PulsateTarget::NONE);
    EXPECT_TRUE(instruction.isComplete);
    EXPECT_NE(instruction.instructionText.find("successful"), std::string::npos);
}

TEST_F(MissionInstructionManagerTest, HandlesPulsateTargetCorrectly) {
    EXPECT_TRUE(missionManager->shouldPulsate(PulsateTarget::DEPTH_THROTTLE));
    EXPECT_FALSE(missionManager->shouldPulsate(PulsateTarget::SONAR_BOX));
    
    auto& state = engine->getState();
    state.depthClearanceMet = true;
    
    EXPECT_FALSE(missionManager->shouldPulsate(PulsateTarget::DEPTH_THROTTLE));
    EXPECT_TRUE(missionManager->shouldPulsate(PulsateTarget::SONAR_BOX));
}

TEST_F(MissionInstructionManagerTest, HandlesResetPhase) {
    setupValidConditions();
    launchHandler->requestAuthorization();
    launchHandler->submitAuthorization(launchHandler->getAuthCode());
    launchHandler->requestReset();
    
    auto instruction = missionManager->getCurrentInstruction();
    
    EXPECT_EQ(instruction.step, MissionStep::ADJUST_DEPTH);
    EXPECT_NE(instruction.instructionText.find("resetting"), std::string::npos);
}
