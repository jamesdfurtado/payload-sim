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
        // Draw rectangular box outline
        DrawRectangleLinesEx(bounds, 1, LIGHTGRAY);
        
        // Draw lightbulb circle (red when false, green when true)
        Color lightColor = isActive ? GREEN : RED;
        int centerX = (int)(bounds.x + 12);
        int centerY = (int)(bounds.y + bounds.height / 2);
        DrawCircle(centerX, centerY, 8, lightColor);
        
        // Draw text label - positioned further right to accommodate longer box
        DrawText(label.c_str(), (int)(bounds.x + 32), (int)(bounds.y + 6), 16, RAYWHITE);
    }

private:
    std::string label;
    bool isActive;
};
