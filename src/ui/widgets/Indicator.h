#pragma once

#include "../Widget.h"
#include <string>

class Indicator : public Widget {
public:
    explicit Indicator(const std::string& label, bool initialState = false)
        : label(std::move(label)), isActive(initialState) {}

    void setState(bool active) { isActive = active; }
    bool getState() const { return isActive; }
    void setLabel(const std::string& newLabel) { label = newLabel; }

    void draw() const override {
        DrawRectangleLinesEx(bounds, 1, LIGHTGRAY);
        
        // status light 
        Color lightColor = isActive ? GREEN : RED;
        int centerX = (int)(bounds.x + 12);
        int centerY = (int)(bounds.y + bounds.height / 2);
        DrawCircle(centerX, centerY, 8, lightColor);
        
        DrawText(label.c_str(), (int)(bounds.x + 32), (int)(bounds.y + 6), 16, RAYWHITE);
    }

private:
    std::string label;
    bool isActive;
};
