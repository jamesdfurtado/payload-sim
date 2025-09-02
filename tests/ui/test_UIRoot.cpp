#include <gtest/gtest.h>
#include <memory>
#include "ui/UIRoot.h"
#include "sim/SimulationEngine.h"
#include "sim/systems/SonarSystem.h"
#include "sim/systems/PowerSystem.h"
#include "sim/systems/DepthSystem.h"
#include "sim/systems/TargetingSystem.h"
#include "sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "sim/systems/EnvironmentSystem.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"
#include "sim/world/MissileManager.h"

class MockSonarSystem : public SonarSystem {
public:
    MockSonarSystem() : SonarSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class MockPowerSystem : public PowerSystem {
public:
    MockPowerSystem() : PowerSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class MockDepthSystem : public DepthSystem {
public:
    MockDepthSystem() : DepthSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class MockTargetingSystem : public TargetingSystem {
public:
    MockTargetingSystem() : TargetingSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class MockLaunchSequenceHandler : public LaunchSequenceHandler {
public:
    MockLaunchSequenceHandler() : LaunchSequenceHandler(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class MockEnvironmentSystem : public EnvironmentSystem {
public:
    MockEnvironmentSystem() : EnvironmentSystem(nullptr) {}
    void update(SimulationState& state, float dt) override {}
};

class UIRootTest : public ::testing::Test {
protected:
    std::unique_ptr<SimulationEngine> engine;
    std::unique_ptr<MockSonarSystem> sonar;
    std::unique_ptr<MockPowerSystem> power;
    std::unique_ptr<MockDepthSystem> depth;
    std::unique_ptr<MockTargetingSystem> targeting;
    std::unique_ptr<MockLaunchSequenceHandler> launchSequence;
    std::unique_ptr<MockEnvironmentSystem> environment;
    std::unique_ptr<ContactManager> contacts;
    std::unique_ptr<CrosshairManager> crosshair;
    std::unique_ptr<MissileManager> missiles;
    std::unique_ptr<UIRoot> uiRoot;

    void SetUp() override {
        engine = std::make_unique<SimulationEngine>();
        sonar = std::make_unique<MockSonarSystem>();
        power = std::make_unique<MockPowerSystem>();
        depth = std::make_unique<MockDepthSystem>();
        targeting = std::make_unique<MockTargetingSystem>();
        launchSequence = std::make_unique<MockLaunchSequenceHandler>();
        environment = std::make_unique<MockEnvironmentSystem>();
        contacts = std::make_unique<ContactManager>();
        crosshair = std::make_unique<CrosshairManager>();
        missiles = std::make_unique<MissileManager>();

        uiRoot = std::make_unique<UIRoot>(
            *engine, sonar.get(), power.get(), depth.get(), targeting.get(),
            launchSequence.get(), environment.get(), contacts.get(),
            crosshair.get(), missiles.get()
        );
    }
};

TEST_F(UIRootTest, UpdatesAllComponentsOnUpdate) {
    uiRoot->update(0.016f);
    
    EXPECT_TRUE(uiRoot->getGuidanceView()->hasUpdated());
    EXPECT_TRUE(uiRoot->getPowerView()->hasUpdated());
    EXPECT_TRUE(uiRoot->getDepthView()->hasUpdated());
    EXPECT_TRUE(uiRoot->getControlPanel()->hasUpdated());
}

TEST_F(UIRootTest, RoutesMouseEventsToCorrectComponents) {
    Vector2 powerClick = {650, 180};
    Vector2 depthClick = {650, 300};
    Vector2 sonarClick = {100, 200};
    Vector2 controlClick = {650, 400};

    uiRoot->simulateMouseClick(powerClick);
    EXPECT_TRUE(uiRoot->getPowerView()->hasReceivedMouseEvent());

    uiRoot->simulateMouseClick(depthClick);
    EXPECT_TRUE(uiRoot->getDepthView()->hasReceivedMouseEvent());

    uiRoot->simulateMouseClick(sonarClick);
    EXPECT_TRUE(uiRoot->getSonarView()->hasReceivedMouseEvent());

    uiRoot->simulateMouseClick(controlClick);
    EXPECT_TRUE(uiRoot->getControlPanel()->hasReceivedMouseEvent());
}

TEST_F(UIRootTest, HandlesMouseInteraction) {
    Vector2 clickPos = {100, 100};
    Vector2 mousePos = {300, 400};
    Vector2 startPos = {100, 100};
    Vector2 dragPos = {150, 150};
    
    uiRoot->simulateMouseClick(clickPos);
    EXPECT_TRUE(uiRoot->getCrosshairManager()->hasReceivedClick());
    
    uiRoot->simulateMouseMove(mousePos);
    EXPECT_EQ(uiRoot->getCrosshairManager()->getMousePosition().x, mousePos.x);
    
    uiRoot->simulateMouseDown(startPos);
    uiRoot->simulateMouseMove(dragPos);
    EXPECT_TRUE(uiRoot->getCrosshairManager()->isDragging());
}

TEST_F(UIRootTest, ShowsPulsatingBorderForDifferentTargets) {
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::POWER_SWITCH, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
    
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::DEPTH_THROTTLE, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
    
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::SONAR_BOX, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
}

TEST_F(UIRootTest, ShowsPulsatingBorderForControlButtons) {
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::AUTHORIZE_BUTTON, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
    
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::ARM_BUTTON, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
    
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::LAUNCH_BUTTON, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
}

TEST_F(UIRootTest, ShowsPulsatingBorderForKeypadArea) {
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::KEYPAD_AREA, true);
    uiRoot->update(0.016f);
    uiRoot->draw();
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
}

TEST_F(UIRootTest, HandlesMultiplePulsatingTargetsSimultaneously) {
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::POWER_SWITCH, true);
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::DEPTH_THROTTLE, true);
    uiRoot->getMissionManager()->setPulsateTarget(PulsateTarget::AUTHORIZE_BUTTON, true);
    
    uiRoot->update(0.016f);
    uiRoot->draw();
    
    EXPECT_TRUE(uiRoot->getPulsatingBorder()->isVisible());
    EXPECT_EQ(uiRoot->getPulsatingBorder()->getActiveTargetCount(), 3);
}