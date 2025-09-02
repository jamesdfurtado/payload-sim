#include <gtest/gtest.h>
#include "ui/views/ControlPanel/AuthCodePanel.h"

class AuthCodePanelTest : public ::testing::Test {
protected:
    std::unique_ptr<AuthCodePanel> authPanel;
    std::string lastSubmittedCode;
    bool submitCalled;
    
    void SetUp() override {
        lastSubmittedCode = "";
        submitCalled = false;
        
        authPanel = std::make_unique<AuthCodePanel>(
            [this](const std::string& code) { 
                lastSubmittedCode = code; 
                submitCalled = true; 
            }
        );
        authPanel->setBounds({10, 10, 200, 100});
    }
};


TEST_F(AuthCodePanelTest, SetsAuthCodeCorrectly) {
    std::string testCode = "1234";
    
    authPanel->setAuthCode(testCode);
    
    EXPECT_EQ(authPanel->getAuthCode(), testCode);
    EXPECT_EQ(authPanel->getDisplayBox()->getText(), testCode);
}

TEST_F(AuthCodePanelTest, SetsEmptyAuthCodeCorrectly) {
    authPanel->setAuthCode("1234");
    authPanel->setAuthCode("");
    
    EXPECT_EQ(authPanel->getAuthCode(), "");
    EXPECT_EQ(authPanel->getDisplayBox()->getText(), "----");
}

TEST_F(AuthCodePanelTest, ClearsInputCorrectly) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    
    authPanel->clearInput();
    
    EXPECT_EQ(authPanel->getInputValue(), "");
}

TEST_F(AuthCodePanelTest, ClearsAuthCodeDisplayCorrectly) {
    authPanel->setAuthCode("1234");
    
    authPanel->clearAuthCodeDisplay();
    
    EXPECT_EQ(authPanel->getAuthCode(), "");
    EXPECT_EQ(authPanel->getDisplayBox()->getText(), "----");
}

TEST_F(AuthCodePanelTest, ChecksInputCompletion) {
    EXPECT_FALSE(authPanel->isInputComplete());
    
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    authPanel->getInputBox()->addCharacter('3');
    authPanel->getInputBox()->addCharacter('4');
    
    EXPECT_TRUE(authPanel->isInputComplete());
}

TEST_F(AuthCodePanelTest, GetsInputValue) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    
    EXPECT_EQ(authPanel->getInputValue(), "12");
}

TEST_F(AuthCodePanelTest, HandlesKeypadInput) {
    char testKey = '5';
    
    authPanel->handleKeypadInput(testKey);
    
    EXPECT_EQ(authPanel->getInputValue(), "5");
}

TEST_F(AuthCodePanelTest, HandlesBackspace) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    
    authPanel->handleBackspace();
    
    EXPECT_EQ(authPanel->getInputValue(), "1");
}

TEST_F(AuthCodePanelTest, AutoSubmitsWhenComplete) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    authPanel->getInputBox()->addCharacter('3');
    authPanel->getInputBox()->addCharacter('4');
    
    authPanel->update(0.016f);
    
    EXPECT_TRUE(submitCalled);
    EXPECT_EQ(lastSubmittedCode, "1234");
}

TEST_F(AuthCodePanelTest, DoesNotAutoSubmitWhenIncomplete) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    
    authPanel->update(0.016f);
    
    EXPECT_FALSE(submitCalled);
}

TEST_F(AuthCodePanelTest, DrawRendersCorrectly) {
    authPanel->draw();
    
    EXPECT_TRUE(authPanel->hasDrawn());
    EXPECT_EQ(authPanel->getLastDrawnBounds().x, 10);
    EXPECT_EQ(authPanel->getLastDrawnBounds().y, 10);
    EXPECT_EQ(authPanel->getLastDrawnBounds().width, 200);
    EXPECT_EQ(authPanel->getLastDrawnBounds().height, 100);
}

TEST_F(AuthCodePanelTest, MouseEventsReturnTrueWhenHandled) {
    Vector2 inputPos = authPanel->getInputBox()->getBounds();
    Vector2 centerPos = {inputPos.x + inputPos.width/2, inputPos.y + inputPos.height/2};
    
    EXPECT_TRUE(authPanel->onMouseDown(centerPos));
    EXPECT_TRUE(authPanel->onMouseUp(centerPos));
    EXPECT_TRUE(authPanel->onMouseMove(centerPos));
}

TEST_F(AuthCodePanelTest, MouseEventsReturnFalseWhenNotHandled) {
    Vector2 outsidePos = {1000, 1000};
    
    EXPECT_FALSE(authPanel->onMouseDown(outsidePos));
    EXPECT_FALSE(authPanel->onMouseUp(outsidePos));
    EXPECT_FALSE(authPanel->onMouseMove(outsidePos));
}

TEST_F(AuthCodePanelTest, UpdatesInputAndDisplayBoxes) {
    authPanel->update(0.016f);
    
    EXPECT_TRUE(authPanel->getInputBox()->hasUpdated());
    EXPECT_TRUE(authPanel->getDisplayBox()->hasUpdated());
}

TEST_F(AuthCodePanelTest, HandlesDifferentBounds) {
    authPanel->setBounds({20, 20, 300, 150});
    
    float expectedMargin = 15;
    float expectedInputHeight = 30;
    float expectedBoxWidth = (300 - 3 * expectedMargin) / 2;
    
    Rectangle inputBounds = authPanel->getInputBox()->getBounds();
    Rectangle displayBounds = authPanel->getDisplayBox()->getBounds();
    
    EXPECT_EQ(inputBounds.x, 20 + expectedMargin);
    EXPECT_EQ(inputBounds.y, 20 + expectedMargin);
    EXPECT_EQ(inputBounds.width, expectedBoxWidth);
    EXPECT_EQ(inputBounds.height, expectedInputHeight);
    
    EXPECT_EQ(displayBounds.x, 20 + expectedMargin + expectedBoxWidth + expectedMargin + 20);
    EXPECT_EQ(displayBounds.y, 20 + expectedMargin);
    EXPECT_EQ(displayBounds.width, expectedBoxWidth);
    EXPECT_EQ(displayBounds.height, expectedInputHeight);
}

TEST_F(AuthCodePanelTest, HandlesMultipleKeypadInputs) {
    std::vector<char> testKeys = {'1', '2', '3', '4'};
    
    for (char key : testKeys) {
        authPanel->handleKeypadInput(key);
    }
    
    EXPECT_EQ(authPanel->getInputValue(), "1234");
    EXPECT_TRUE(authPanel->isInputComplete());
}

TEST_F(AuthCodePanelTest, HandlesMultipleBackspaces) {
    authPanel->getInputBox()->addCharacter('1');
    authPanel->getInputBox()->addCharacter('2');
    authPanel->getInputBox()->addCharacter('3');
    authPanel->getInputBox()->addCharacter('4');
    
    authPanel->handleBackspace();
    authPanel->handleBackspace();
    
    EXPECT_EQ(authPanel->getInputValue(), "12");
    EXPECT_FALSE(authPanel->isInputComplete());
}

TEST_F(AuthCodePanelTest, HandlesAuthCodeChanges) {
    std::vector<std::string> testCodes = {"1111", "2222", "3333", "4444", ""};
    
    for (const auto& code : testCodes) {
        authPanel->setAuthCode(code);
        EXPECT_EQ(authPanel->getAuthCode(), code);
        
        if (code.empty()) {
            EXPECT_EQ(authPanel->getDisplayBox()->getText(), "----");
        } else {
            EXPECT_EQ(authPanel->getDisplayBox()->getText(), code);
        }
    }
}

TEST_F(AuthCodePanelTest, HandlesNullCallback) {
    AuthCodePanel nullPanel([](const std::string&){});
    nullPanel.setBounds({10, 10, 200, 100});
    
    nullPanel.getInputBox()->addCharacter('1');
    nullPanel.getInputBox()->addCharacter('2');
    nullPanel.getInputBox()->addCharacter('3');
    nullPanel.getInputBox()->addCharacter('4');
    
    nullPanel.update(0.016f);
    
    EXPECT_TRUE(nullPanel.hasDrawn());
}
