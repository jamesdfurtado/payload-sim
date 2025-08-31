#include <gtest/gtest.h>
#include "ui/widgets/Switch.h"

class SwitchTest : public ::testing::Test {
protected:
    bool switchToggled = false;
    bool lastState = false;
    
    void SetUp() override {
        switchToggled = false;
        lastState = false;
    }
    
    void switchCallback(bool state) {
        switchToggled = true;
        lastState = state;
    }
};

TEST_F(SwitchTest, DefaultState) {
    Switch switchWidget(false, [this](bool state) { switchCallback(state); });
    EXPECT_FALSE(switchWidget.getState());
}

TEST_F(SwitchTest, InitialState) {
    Switch switchWidget(true, [this](bool state) { switchCallback(state); });
    EXPECT_TRUE(switchWidget.getState());
}

TEST_F(SwitchTest, StateToggle) {
    // TODO: Test state toggling via mouse interaction
    EXPECT_TRUE(true); // Placeholder
}

TEST_F(SwitchTest, QuietStateChange) {
    Switch switchWidget(false, [this](bool state) { switchCallback(state); });
    switchWidget.setStateQuiet(true);
    
    EXPECT_TRUE(switchWidget.getState());
    EXPECT_FALSE(switchToggled); // Callback should not have been called
}
