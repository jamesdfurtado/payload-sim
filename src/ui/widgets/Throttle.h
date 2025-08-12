#pragma once

#include "../Widget.h"
#include <functional>
#include <algorithm>

class Throttle : public Widget {
public:
    explicit Throttle(std::function<void(float)> onChange = nullptr)
        : value(0.5f), onChange(std::move(onChange)), dragging(false) {}

    void setValue(float newValue) { 
        value = std::clamp(newValue, 0.0f, 1.0f);
        if (onChange) onChange(value);
    }
    float getValue() const { return value; }
    void setOnChange(std::function<void(float)> fn) { onChange = std::move(fn); }

    void draw() const override {
        // Draw throttle background
        DrawRectangleRec(bounds, Fade(DARKGRAY, 0.8f));
        DrawRectangleLinesEx(bounds, 2, LIME);
        
        // Draw center line (neutral position)
        float centerY = bounds.y + bounds.height / 2;
        DrawLine((int)bounds.x, (int)centerY, (int)(bounds.x + bounds.width), (int)centerY, YELLOW);
        
        // Draw throttle track
        Rectangle trackRect = { bounds.x + bounds.width / 2 - 2, bounds.y + 5, 4, bounds.height - 10 };
        DrawRectangleRec(trackRect, DARKGRAY);
        
        // Draw throttle handle
        float handleSize = 12;
        float handleY = bounds.y + 5 + (1.0f - value) * (bounds.height - 10);
        Vector2 handlePos = { bounds.x + bounds.width / 2, handleY };
        DrawCircleV(handlePos, handleSize, dragging ? ORANGE : RAYWHITE);
        DrawCircleLinesV(handlePos, handleSize, BLACK);
        
        // Draw labels
        DrawText("UP", (int)(bounds.x + bounds.width + 20), (int)bounds.y + 5, 16, LIGHTGRAY);
        DrawText("NEUTRAL", (int)(bounds.x + bounds.width + 20), (int)centerY - 8, 16, YELLOW);
        DrawText("DOWN", (int)(bounds.x + bounds.width + 20), (int)(bounds.y + bounds.height - 20), 16, LIGHTGRAY);
    }

    bool onMouseDown(Vector2 pos) override {
        // Check if click is within the expanded handle area (including handle size)
        if (isClickInHandleArea(pos)) {
            dragging = true;
            updateValueFromMouse(pos);
            return true;
        }
        return false;
    }

    bool onMouseUp(Vector2 pos) override {
        if (dragging) {
            dragging = false;
            updateValueFromMouse(pos);
            return true;
        }
        return false;
    }

    bool onMouseMove(Vector2 pos) override {
        if (dragging) {
            updateValueFromMouse(pos);
            return true;
        }
        return false;
    }

    void update(float dt) override {
        // If we're dragging, continuously update based on current mouse position
        if (dragging) {
            Vector2 mousePos = GetMousePosition();
            updateValueFromMouse(mousePos);
        }
    }

private:
    bool isClickInHandleArea(Vector2 pos) const {
        // Calculate handle position and size
        float handleSize = 12;
        float handleY = bounds.y + 5 + (1.0f - value) * (bounds.height - 10);
        Vector2 handlePos = { bounds.x + bounds.width / 2, handleY };
        
        // Check if click is within the handle circle (expanded clickable area)
        float dx = pos.x - handlePos.x;
        float dy = pos.y - handlePos.y;
        float distanceSquared = dx * dx + dy * dy;
        
        return distanceSquared <= (handleSize * handleSize);
    }
    
    void updateValueFromMouse(Vector2 pos) {
        float relativeY = (pos.y - bounds.y - 5) / (bounds.height - 10);
        value = std::clamp(1.0f - relativeY, 0.0f, 1.0f);
        if (onChange) onChange(value);
    }

    float value; // 0.0f = DOWN, 0.5f = NEUTRAL, 1.0f = UP
    std::function<void(float)> onChange;
    bool dragging;
};
