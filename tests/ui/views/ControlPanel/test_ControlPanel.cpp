#include <gtest/gtest.h>
#include "ui/views/ControlPanel/ControlPanel.h"
#include "sim/SimulationEngine.h"
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"

class MockLaunchSequenceHandler : public LaunchSequenceHandler {
public:
    MockLaunchSequenceHandler() : LaunchSequenceHandler(nullptr) {}
    void update(SimulationState& state, float dt) override {}
    
    std::string currentPhase = "IDLE";
    std::string authCode = "";
    bool authorizationRequested = false;
    bool armRequested = false;
    bool launchRequested = false;
    bool resetRequested = false;
    bool authSubmitted = false;
    std::string submittedAuthCode = "";
    
    const char* getCurrentPhaseString() const override { return currentPhase.c_str(); }
    const std::string& getAuthCode() const override { return authCode; }
    void requestAuthorization() override { authorizationRequested = true; }
    void requestArm() override { armRequested = true; }
    void requestLaunch() override { launchRequested = true; }
    void requestReset() override { resetRequested = true; }
    void submitAuthorization(const std::string& code) override { 
        authSubmitted = true; 
        submittedAuthCode = code; 
    }
};

class ControlPanelTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<MockLaunchSequenceHandler> sequenceHandler;
    std::unique_ptr<ControlPanel> controlPanel;
    
    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        sequenceHandler = std::make_unique<MockLaunchSequenceHandler>();
        controlPanel = std::make_unique<ControlPanel>(*engine, sequenceHandler.get());
        controlPanel->setBounds({10, 10, 400, 600});
    }
};

TEST_F(ControlPanelTest, SequenceHandlerIntegration) {
    std::vector<std::string> testCodes = {"1234", "5678", "9999", "0000"};
    std::vector<std::string> phases = {"IDLE", "AUTHORIZED", "ARMED", "LAUNCHING", "COMPLETE"};
    
    for (const auto& code : testCodes) {
        controlPanel->handleAuthCodeSubmit(code);
        EXPECT_EQ(sequenceHandler->submittedAuthCode, code);
    }
    
    for (const auto& phase : phases) {
        sequenceHandler->currentPhase = phase;
        controlPanel->update(0.016f);
        EXPECT_EQ(controlPanel->getPhaseDisplay()->getCurrentPhase(), phase);
    }
}

TEST_F(ControlPanelTest, AuthCodeManagement) {
    sequenceHandler->authCode = "5678";
    controlPanel->update(0.016f);
    EXPECT_EQ(controlPanel->getAuthCodePanel()->getAuthCode(), "5678");
    
    sequenceHandler->authCode = "";
    controlPanel->update(0.016f);
    EXPECT_TRUE(controlPanel->getAuthCodePanel()->isInputCleared());
    EXPECT_TRUE(controlPanel->getAuthCodePanel()->isDisplayCleared());
    
    controlPanel->getAuthCodePanel()->addCharacter('1');
    controlPanel->getAuthCodePanel()->addCharacter('2');
    controlPanel->handleBackspace();
    EXPECT_EQ(controlPanel->getAuthCodePanel()->getInputValue(), "1");
}

TEST_F(ControlPanelTest, EventRouting) {
    Vector2 leftPanelPos = {50, 50};
    Vector2 rightPanelPos = {300, 50};
    
    EXPECT_TRUE(controlPanel->onMouseDown(leftPanelPos));
    EXPECT_TRUE(controlPanel->onMouseDown(rightPanelPos));
    
    char testKey = '5';
    controlPanel->handleKeypadInput(testKey);
    EXPECT_EQ(controlPanel->getAuthCodePanel()->getLastInputKey(), testKey);
}

TEST_F(ControlPanelTest, ComponentUpdates) {
    controlPanel->update(0.016f);
    
    EXPECT_TRUE(controlPanel->getLaunchSequencePanel()->hasUpdated());
    EXPECT_TRUE(controlPanel->getKeypadPanel()->hasUpdated());
    EXPECT_TRUE(controlPanel->getAuthCodePanel()->hasUpdated());
}

TEST_F(ControlPanelTest, NullHandlerSafety) {
    ControlPanel nullPanel(*engine, nullptr);
    nullPanel.setBounds({10, 10, 400, 600});
    
    nullPanel.handleAuthCodeSubmit("1234");
    nullPanel.update(0.016f);
    
    EXPECT_TRUE(nullPanel.hasDrawn());
}
