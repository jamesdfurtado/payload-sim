#include <gtest/gtest.h>
#include "ui/widgets/Button.h"
#include <raylib.h>

class ButtonTest : public ::testing::Test {
protected:
    bool buttonClicked = false;
    char keyPressed = '\0';
    int clickCount = 0;
    
    void SetUp() override {
        buttonClicked = false;
        keyPressed = '\0';
        clickCount = 0;
    }
    
    void buttonCallback() {
        buttonClicked = true;
        clickCount++;
    }
    
    void keyCallback(char key) {
        keyPressed = key;
        clickCount++;
    }
};

TEST_F(ButtonTest, BasicCreation) {
    Button button("Test", [this]() { buttonCallback(); });
    EXPECT_FALSE(buttonClicked);
    EXPECT_STREQ(button.getLabel().c_str(), "Test");
    EXPECT_TRUE(button.isEnabled());
    EXPECT_EQ(button.getState(), Button::State::Normal);
}

TEST_F(ButtonTest, EnabledDisabledStates) {
    Button button("Test", [this]() { buttonCallback(); });
    
    // Default is enabled
    EXPECT_TRUE(button.isEnabled());
    
    // Disable button
    button.setEnabled(false);
    EXPECT_FALSE(button.isEnabled());
    
    // Re-enable
    button.setEnabled(true);
    EXPECT_TRUE(button.isEnabled());
}

TEST_F(ButtonTest, MouseInteractionBasic) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    
    Vector2 insidePos = {50, 35}; // Inside button
    Vector2 outsidePos = {200, 200}; // Outside button
    
    // Mouse down inside
    EXPECT_TRUE(button.onMouseDown(insidePos));
    EXPECT_EQ(button.getState(), Button::State::Pressed);
    
    // Mouse up inside (should trigger callback)
    EXPECT_TRUE(button.onMouseUp(insidePos));
    EXPECT_TRUE(buttonClicked);
    EXPECT_EQ(clickCount, 1);
    
    // State should return to normal
    EXPECT_EQ(button.getState(), Button::State::Normal);
}

TEST_F(ButtonTest, MouseInteractionOutside) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    
    Vector2 outsidePos = {200, 200};
    
    // Mouse down outside should not register
    EXPECT_FALSE(button.onMouseDown(outsidePos));
    EXPECT_EQ(button.getState(), Button::State::Normal);
    
    // Mouse up outside should not trigger callback
    EXPECT_FALSE(button.onMouseUp(outsidePos));
    EXPECT_FALSE(buttonClicked);
}

TEST_F(ButtonTest, MouseDragOffButton) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    
    Vector2 insidePos = {50, 35};
    Vector2 outsidePos = {200, 200};
    
    // Press inside
    EXPECT_TRUE(button.onMouseDown(insidePos));
    EXPECT_EQ(button.getState(), Button::State::Pressed);
    
    // Release outside (should not trigger callback)
    EXPECT_FALSE(button.onMouseUp(outsidePos));
    EXPECT_FALSE(buttonClicked);
    EXPECT_EQ(button.getState(), Button::State::Normal);
}

TEST_F(ButtonTest, HoverState) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    
    Vector2 insidePos = {50, 35};
    Vector2 outsidePos = {200, 200};
    
    // Move mouse inside (should hover)
    button.onMouseMove(insidePos);
    EXPECT_EQ(button.getState(), Button::State::Hovered);
    
    // Move mouse outside (should return to normal)
    button.onMouseMove(outsidePos);
    EXPECT_EQ(button.getState(), Button::State::Normal);
}

TEST_F(ButtonTest, DisabledButtonNoInteraction) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    button.setEnabled(false);
    
    Vector2 insidePos = {50, 35};
    
    // Disabled button should not respond to any interaction
    EXPECT_FALSE(button.onMouseDown(insidePos));
    EXPECT_FALSE(button.onMouseUp(insidePos));
    EXPECT_FALSE(button.onMouseMove(insidePos));
    
    EXPECT_FALSE(buttonClicked);
    EXPECT_EQ(button.getState(), Button::State::Normal);
}

TEST_F(ButtonTest, KeypadButtonFunctionality) {
    Button keypadButton('5', [this](char key) { keyCallback(key); });
    keypadButton.setBounds({10, 10, 50, 50});
    
    Vector2 insidePos = {35, 35};
    
    // Press and release keypad button
    EXPECT_TRUE(keypadButton.onMouseDown(insidePos));
    EXPECT_TRUE(keypadButton.onMouseUp(insidePos));
    
    // Should have triggered key callback with '5'
    EXPECT_EQ(keyPressed, '5');
    EXPECT_EQ(clickCount, 1);
}

TEST_F(ButtonTest, ActiveAndCompletedStates) {
    Button button("Test", [this]() { buttonCallback(); });
    
    // Test active state
    button.setActive(true);
    EXPECT_TRUE(button.isActive());
    
    button.setActive(false);
    EXPECT_FALSE(button.isActive());
    
    // Test completed state
    button.setCompleted(true);
    EXPECT_TRUE(button.isCompleted());
    
    button.setCompleted(false);
    EXPECT_FALSE(button.isCompleted());
}

TEST_F(ButtonTest, LaunchSequenceButtonScenario) {
    // Test scenario similar to launch sequence buttons
    Button authorizeButton("AUTHORIZE", [this]() { buttonCallback(); }, false); // Initially disabled
    authorizeButton.setBounds({100, 100, 120, 40});
    
    Vector2 clickPos = {160, 120};
    
    // Should not respond when disabled
    EXPECT_FALSE(authorizeButton.isEnabled());
    EXPECT_FALSE(authorizeButton.onMouseDown(clickPos));
    EXPECT_FALSE(authorizeButton.onMouseUp(clickPos));
    EXPECT_FALSE(buttonClicked);
    
    // Enable button (like when launch conditions are met)
    authorizeButton.setEnabled(true);
    
    // Should now respond
    EXPECT_TRUE(authorizeButton.onMouseDown(clickPos));
    EXPECT_TRUE(authorizeButton.onMouseUp(clickPos));
    EXPECT_TRUE(buttonClicked);
    
    // Mark as completed (like after successful authorization)
    authorizeButton.setCompleted(true);
    EXPECT_TRUE(authorizeButton.isCompleted());
}

TEST_F(ButtonTest, StyleCustomization) {
    Button button("Test", []() {});
    
    // Test color customization
    button.setColors(RED, GREEN, BLUE);
    
    // Test style properties
    button.setTextSize(24);
    button.setBorderThickness(3);
    
    // Test passes if no crashes occur
    EXPECT_TRUE(true);
}

TEST_F(ButtonTest, BoundsManagement) {
    Button button("Test", []() {});
    button.setBounds({25, 50, 150, 75});
    
    Rectangle bounds = button.getBounds();
    EXPECT_FLOAT_EQ(bounds.x, 25);
    EXPECT_FLOAT_EQ(bounds.y, 50);
    EXPECT_FLOAT_EQ(bounds.width, 150);
    EXPECT_FLOAT_EQ(bounds.height, 75);
}

TEST_F(ButtonTest, MultipleClicksHandling) {
    Button button("Test", [this]() { buttonCallback(); });
    button.setBounds({10, 10, 100, 50});
    
    Vector2 clickPos = {50, 35};
    
    // Multiple click cycles
    for (int i = 0; i < 5; i++) {
        button.onMouseDown(clickPos);
        button.onMouseUp(clickPos);
    }
    
    EXPECT_EQ(clickCount, 5);
    EXPECT_TRUE(buttonClicked);
}
