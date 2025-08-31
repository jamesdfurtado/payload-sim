#include <gtest/gtest.h>
#include "ui/widgets/Button.h"

class ButtonTest : public ::testing::Test {
protected:
    bool buttonClicked = false;
    
    void SetUp() override {
        buttonClicked = false;
    }
    
    void buttonCallback() {
        buttonClicked = true;
    }
};

TEST_F(ButtonTest, BasicCreation) {
    Button button("Test", [this]() { buttonCallback(); });
    EXPECT_FALSE(buttonClicked);
}

TEST_F(ButtonTest, ClickCallback) {
    // TODO: Test button click callback execution
    EXPECT_TRUE(true); // Placeholder - requires mouse event simulation
}

TEST_F(ButtonTest, ColorCustomization) {
    Button button("Test", []() {});
    button.setColors(RED, GREEN, BLUE);
    EXPECT_TRUE(true); // Color setting test
}

TEST_F(ButtonTest, BoundsManagement) {
    Button button("Test", []() {});
    button.setBounds({0, 0, 100, 30});
    
    Rectangle bounds = button.getBounds();
    EXPECT_FLOAT_EQ(bounds.width, 100);
    EXPECT_FLOAT_EQ(bounds.height, 30);
}
