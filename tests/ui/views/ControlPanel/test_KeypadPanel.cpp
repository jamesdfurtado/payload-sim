#include <gtest/gtest.h>
#include "ui/views/ControlPanel/KeypadPanel.h"

class KeypadPanelTest : public ::testing::Test {
protected:
    std::unique_ptr<KeypadPanel> keypadPanel;
    char lastPressedKey;
    bool backspaceCalled;
    
    void SetUp() override {
        lastPressedKey = '\0';
        backspaceCalled = false;
        
        keypadPanel = std::make_unique<KeypadPanel>(
            [this](char key) { lastPressedKey = key; },
            [this]() { backspaceCalled = true; }
        );
        keypadPanel->setBounds({10, 10, 120, 160});
    }
};


TEST_F(KeypadPanelTest, HandlesKeyPress) {
    Vector2 buttonPos = keypadPanel->getButtonBounds(0);
    Vector2 centerPos = {buttonPos.x + buttonPos.width/2, buttonPos.y + buttonPos.height/2};
    
    keypadPanel->onMouseDown(centerPos);
    keypadPanel->onMouseUp(centerPos);
    
    EXPECT_EQ(lastPressedKey, '7');
}

TEST_F(KeypadPanelTest, HandlesBackspacePress) {
    Vector2 backspacePos = keypadPanel->getButtonBounds(10);
    Vector2 centerPos = {backspacePos.x + backspacePos.width/2, backspacePos.y + backspacePos.height/2};
    
    keypadPanel->onMouseDown(centerPos);
    keypadPanel->onMouseUp(centerPos);
    
    EXPECT_TRUE(backspaceCalled);
}

TEST_F(KeypadPanelTest, HandlesAllNumberKeys) {
    std::vector<char> expectedKeys = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '0'};
    
    for (size_t i = 0; i < expectedKeys.size(); i++) {
        Vector2 buttonPos = keypadPanel->getButtonBounds(i);
        Vector2 centerPos = {buttonPos.x + buttonPos.width/2, buttonPos.y + buttonPos.height/2};
        
        keypadPanel->onMouseDown(centerPos);
        keypadPanel->onMouseUp(centerPos);
        
        EXPECT_EQ(lastPressedKey, expectedKeys[i]);
    }
}

TEST_F(KeypadPanelTest, DrawRendersCorrectly) {
    keypadPanel->draw();
    
    EXPECT_TRUE(keypadPanel->hasDrawn());
    EXPECT_EQ(keypadPanel->getLastDrawnBounds().x, 10);
    EXPECT_EQ(keypadPanel->getLastDrawnBounds().y, 10);
    EXPECT_EQ(keypadPanel->getLastDrawnBounds().width, 120);
    EXPECT_EQ(keypadPanel->getLastDrawnBounds().height, 160);
}

TEST_F(KeypadPanelTest, DrawShowsCorrectTitle) {
    keypadPanel->draw();
    
    EXPECT_EQ(keypadPanel->getLastDrawnTitle(), "KEYPAD");
    EXPECT_EQ(keypadPanel->getLastDrawnTitleColor().r, RAYWHITE.r);
}

TEST_F(KeypadPanelTest, DrawsAllButtons) {
    keypadPanel->draw();
    
    EXPECT_EQ(keypadPanel->getLastDrawnButtonCount(), 11);
}

TEST_F(KeypadPanelTest, MouseEventsReturnTrueWhenHandled) {
    Vector2 buttonPos = keypadPanel->getButtonBounds(0);
    Vector2 centerPos = {buttonPos.x + buttonPos.width/2, buttonPos.y + buttonPos.height/2};
    
    EXPECT_TRUE(keypadPanel->onMouseDown(centerPos));
    EXPECT_TRUE(keypadPanel->onMouseUp(centerPos));
    EXPECT_TRUE(keypadPanel->onMouseMove(centerPos));
}

TEST_F(KeypadPanelTest, MouseEventsReturnFalseWhenNotHandled) {
    Vector2 outsidePos = {1000, 1000};
    
    EXPECT_FALSE(keypadPanel->onMouseDown(outsidePos));
    EXPECT_FALSE(keypadPanel->onMouseUp(outsidePos));
    EXPECT_FALSE(keypadPanel->onMouseMove(outsidePos));
}

TEST_F(KeypadPanelTest, UpdatesAllButtons) {
    keypadPanel->update(0.016f);
    
    for (int i = 0; i < keypadPanel->getButtonCount(); i++) {
        EXPECT_TRUE(keypadPanel->getButtonHasUpdated(i));
    }
}

TEST_F(KeypadPanelTest, HandlesMultipleKeyPresses) {
    std::vector<char> testKeys = {'1', '2', '3', '4', '5'};
    
    for (size_t i = 0; i < testKeys.size(); i++) {
        int buttonIndex = i % 10;
        Vector2 buttonPos = keypadPanel->getButtonBounds(buttonIndex);
        Vector2 centerPos = {buttonPos.x + buttonPos.width/2, buttonPos.y + buttonPos.height/2};
        
        keypadPanel->onMouseDown(centerPos);
        keypadPanel->onMouseUp(centerPos);
        
        EXPECT_EQ(lastPressedKey, testKeys[i]);
    }
}

TEST_F(KeypadPanelTest, HandlesMultipleBackspacePresses) {
    for (int i = 0; i < 5; i++) {
        Vector2 backspacePos = keypadPanel->getButtonBounds(10);
        Vector2 centerPos = {backspacePos.x + backspacePos.width/2, backspacePos.y + backspacePos.height/2};
        
        keypadPanel->onMouseDown(centerPos);
        keypadPanel->onMouseUp(centerPos);
        
        EXPECT_TRUE(backspaceCalled);
    }
}

TEST_F(KeypadPanelTest, HandlesNullCallbacks) {
    KeypadPanel nullPanel([](char){}, [](){});
    nullPanel.setBounds({10, 10, 120, 160});
    
    Vector2 buttonPos = nullPanel.getButtonBounds(0);
    Vector2 centerPos = {buttonPos.x + buttonPos.width/2, buttonPos.y + buttonPos.height/2};
    
    nullPanel.onMouseDown(centerPos);
    nullPanel.onMouseUp(centerPos);
    
    EXPECT_TRUE(nullPanel.hasDrawn());
}