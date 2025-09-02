#include <gtest/gtest.h>
#include "ui/widgets/TextBox.h"

class TextBoxTest : public ::testing::Test {
protected:
    TextBox textBox;
    
    void SetUp() override {
        textBox = TextBox(TextBox::Mode::INPUT, 4);
        textBox.setBounds({10, 10, 100, 30});
    }
};

TEST_F(TextBoxTest, InitializesWithCorrectMode) {
    TextBox displayBox(TextBox::Mode::DISPLAY, 6);
    TextBox inputBox(TextBox::Mode::INPUT, 4);
    
    EXPECT_EQ(displayBox.getMode(), TextBox::Mode::DISPLAY);
    EXPECT_EQ(inputBox.getMode(), TextBox::Mode::INPUT);
}

TEST_F(TextBoxTest, AddCharacterRespectsMaxLength) {
    textBox.addCharacter('1');
    textBox.addCharacter('2');
    textBox.addCharacter('3');
    textBox.addCharacter('4');
    textBox.addCharacter('5');
    
    EXPECT_EQ(textBox.getText(), "1234");
    EXPECT_EQ(textBox.getCurrentLength(), 4);
    EXPECT_TRUE(textBox.isComplete());
}

TEST_F(TextBoxTest, AddCharacterOnlyAcceptsDigits) {
    textBox.addCharacter('a');
    textBox.addCharacter('1');
    textBox.addCharacter('b');
    textBox.addCharacter('2');
    
    EXPECT_EQ(textBox.getText(), "12");
    EXPECT_EQ(textBox.getCurrentLength(), 2);
}

TEST_F(TextBoxTest, RemoveCharacterWorks) {
    textBox.addCharacter('1');
    textBox.addCharacter('2');
    textBox.addCharacter('3');
    
    textBox.removeCharacter();
    
    EXPECT_EQ(textBox.getText(), "12");
    EXPECT_EQ(textBox.getCurrentLength(), 2);
}

TEST_F(TextBoxTest, ClearEmptiesTextBox) {
    textBox.addCharacter('1');
    textBox.addCharacter('2');
    textBox.addCharacter('3');
    
    textBox.clear();
    
    EXPECT_TRUE(textBox.isEmpty());
    EXPECT_EQ(textBox.getCurrentLength(), 0);
    EXPECT_EQ(textBox.getText(), "");
}
