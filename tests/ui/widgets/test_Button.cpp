#include <gtest/gtest.h>
#include "ui/widgets/Button.h"

class ButtonTest : public ::testing::Test {
protected:
    Button button;
    bool callbackCalled;
    char keyPressed;
    
    void SetUp() override {
        callbackCalled = false;
        keyPressed = '\0';
        button = Button("Test", [this]() { callbackCalled = true; });
        button.setBounds({10, 10, 100, 30});
    }
};

TEST_F(ButtonTest, MouseInteractionTriggersCallback) {
    Vector2 pos = {50, 25};
    
    button.onMouseDown(pos);
    button.onMouseUp(pos);
    
    EXPECT_TRUE(callbackCalled);
}

TEST_F(ButtonTest, DisabledStatePreventsInteraction) {
    button.setEnabled(false);
    Vector2 pos = {50, 25};
    
    button.onMouseDown(pos);
    button.onMouseUp(pos);
    
    EXPECT_FALSE(callbackCalled);
}

TEST_F(ButtonTest, KeypadButtonTriggersKeyCallback) {
    char pressedKey = '\0';
    Button keypadButton('5', [&pressedKey](char key) { pressedKey = key; });
    keypadButton.setBounds({10, 10, 50, 50});
    
    Vector2 pos = {25, 25};
    keypadButton.onMouseDown(pos);
    keypadButton.onMouseUp(pos);
    
    EXPECT_EQ(pressedKey, '5');
}