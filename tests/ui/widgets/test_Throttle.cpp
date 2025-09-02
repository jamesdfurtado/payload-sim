#include <gtest/gtest.h>
#include "ui/widgets/Throttle.h"

class ThrottleTest : public ::testing::Test {
protected:
    Throttle throttle;
    float callbackValue;
    bool callbackCalled;
    
    void SetUp() override {
        callbackValue = 0.0f;
        callbackCalled = false;
        throttle = Throttle([this](float value) { 
            callbackValue = value; 
            callbackCalled = true; 
        });
        throttle.setBounds({10, 10, 20, 100});
    }
};

TEST_F(ThrottleTest, InitializesWithDefaultValue) {
    EXPECT_FLOAT_EQ(throttle.getValue(), 0.5f);
}

TEST_F(ThrottleTest, SetValueClampsToValidRange) {
    throttle.setValue(1.5f);
    EXPECT_FLOAT_EQ(throttle.getValue(), 1.0f);
    
    throttle.setValue(-0.5f);
    EXPECT_FLOAT_EQ(throttle.getValue(), 0.0f);
    
    throttle.setValue(0.75f);
    EXPECT_FLOAT_EQ(throttle.getValue(), 0.75f);
}

TEST_F(ThrottleTest, SetValueTriggersCallback) {
    throttle.setValue(0.8f);
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_FLOAT_EQ(callbackValue, 0.8f);
}

TEST_F(ThrottleTest, MouseDragUpdatesValue) {
    Vector2 handlePos = {20, 60};
    Vector2 newPos = {20, 40};
    
    throttle.onMouseDown(handlePos);
    throttle.onMouseMove(newPos);
    
    EXPECT_TRUE(throttle.isDragging());
    EXPECT_GT(throttle.getValue(), 0.5f);
}
