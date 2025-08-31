#include <gtest/gtest.h>
#include "ui/Widget.h"

// Mock widget for testing base functionality
class TestWidget : public Widget {
public:
    void draw() const override {}
    bool onMouseDown(Vector2 pos) override { 
        lastMousePos = pos; 
        return true; 
    }
    
    Vector2 lastMousePos{0, 0};
};

class WidgetTest : public ::testing::Test {
protected:
    TestWidget widget;
    
    void SetUp() override {
        widget.setBounds({10, 20, 100, 50});
    }
};

TEST_F(WidgetTest, BoundsManagement) {
    Rectangle bounds = widget.getBounds();
    EXPECT_FLOAT_EQ(bounds.x, 10);
    EXPECT_FLOAT_EQ(bounds.y, 20);
    EXPECT_FLOAT_EQ(bounds.width, 100);
    EXPECT_FLOAT_EQ(bounds.height, 50);
}

TEST_F(WidgetTest, ContainsPoint) {
    EXPECT_TRUE(widget.contains({50, 40}));   // Inside
    EXPECT_FALSE(widget.contains({5, 40}));   // Left of bounds
    EXPECT_FALSE(widget.contains({120, 40})); // Right of bounds
    EXPECT_FALSE(widget.contains({50, 15}));  // Above bounds
    EXPECT_FALSE(widget.contains({50, 75}));  // Below bounds
}

TEST_F(WidgetTest, MouseEventHandling) {
    Vector2 testPos{25, 35};
    widget.onMouseDown(testPos);
    
    EXPECT_FLOAT_EQ(widget.lastMousePos.x, 25);
    EXPECT_FLOAT_EQ(widget.lastMousePos.y, 35);
}
