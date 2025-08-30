#pragma once

#include "../Widget.h"
#include <functional>

class Switch : public Widget {
public:
    explicit Switch(bool initialState = false, std::function<void(bool)> onChange = nullptr)
        : isOn(initialState), onChange(std::move(onChange)) {}

    void setState(bool state) { 
        if (isOn != state) {
            isOn = state;
            if (onChange) onChange(isOn);
        }
    }
    
    void setStateQuiet(bool state) {
        isOn = state; // Set state without triggering callback
    }
    bool getState() const { return isOn; }
    void setOnChange(std::function<void(bool)> fn) { onChange = std::move(fn); }

    void draw() const override {
        // Draw slider background (grey when OFF, green when ON)
        Color sliderColor = isOn ? GREEN : DARKGRAY;
        DrawRectangleRec(bounds, sliderColor);
        DrawRectangleLinesEx(bounds, 2, isOn ? RAYWHITE : GRAY);
        
        // Draw slider handle (slides left/right)
        float handleX = isOn ? (bounds.x + bounds.width - 20) : (bounds.x + 2);
        Rectangle handleRect = { handleX, bounds.y + 2, 16, bounds.height - 4 };
        DrawRectangleRec(handleRect, RAYWHITE);
        DrawRectangleLinesEx(handleRect, 1, BLACK);
        
        // Draw ON/OFF text
        const char* buttonText = isOn ? "ON" : "OFF";
        int textWidth = MeasureText(buttonText, 16);
        int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
        int textY = (int)(bounds.y + (bounds.height - 16) / 2);
        DrawText(buttonText, textX, textY, 16, isOn ? RAYWHITE : LIGHTGRAY);
    }

    bool onMouseUp(Vector2 pos) override {
        if (contains(pos)) {
            setState(!isOn);
            return true;
        }
        return false;
    }

private:
    bool isOn;
    std::function<void(bool)> onChange;
};
