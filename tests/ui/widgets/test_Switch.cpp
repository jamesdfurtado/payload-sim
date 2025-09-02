#include <gtest/gtest.h>
#include "ui/widgets/Switch.h"

class SwitchTest : public ::testing::Test {
protected:
    Switch switchWidget;
    bool callbackCalled;
    bool callbackValue;
    
    void SetUp() override {
        callbackCalled = false;
        callbackValue = false;
        switchWidget = Switch(false, [this](bool value) { 
            callbackCalled = true; 
            callbackValue = value; 
        });
        switchWidget.setBounds({10, 10, 60, 30});
    }
};

TEST_F(SwitchTest, InitializesWithCorrectState) {
    Switch offSwitch(false);
    Switch onSwitch(true);
    
    EXPECT_FALSE(offSwitch.getState());
    EXPECT_TRUE(onSwitch.getState());
}

TEST_F(SwitchTest, SetStateChangesState) {
    switchWidget.setState(true);
    EXPECT_TRUE(switchWidget.getState());
    
    switchWidget.setState(false);
    EXPECT_FALSE(switchWidget.getState());
}

TEST_F(SwitchTest, SetStateTriggersCallback) {
    switchWidget.setState(true);
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_TRUE(callbackValue);
}

TEST_F(SwitchTest, MouseClickTogglesState) {
    bool initialState = switchWidget.getState();
    Vector2 pos = {40, 25};
    
    switchWidget.onMouseUp(pos);
    
    EXPECT_NE(switchWidget.getState(), initialState);
    EXPECT_TRUE(callbackCalled);
}
