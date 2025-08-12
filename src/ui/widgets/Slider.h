#pragma once

#include "../Widget.h"
#include <functional>

class Slider : public Widget {
public:
    explicit Slider(std::function<void(float)> onChange, float initial = 0.5f)
        : onChange(std::move(onChange)), value(initial) {}

    void draw() const override {
        DrawRectangleRec(bounds, DARKBROWN);
        DrawRectangleLinesEx(bounds, 2, RAYWHITE);
        float knobX = bounds.x + value * bounds.width;
        DrawCircle((int)knobX, (int)(bounds.y + bounds.height/2), bounds.height/2 - 2, YELLOW);
    }

    bool onMouseDown(Vector2 pos) override {
        if (!contains(pos)) return false;
        dragging = true;
        return onMouseUp(pos); // update value immediately
    }

    bool onMouseUp(Vector2 pos) override {
        if (!dragging && !contains(pos)) return false;
        dragging = false;
        float nx = (pos.x - bounds.x) / bounds.width;
        value = nx < 0 ? 0 : (nx > 1 ? 1 : nx);
        if (onChange) onChange(value);
        return true;
    }

    float getValue() const { return value; }

private:
    std::function<void(float)> onChange;
    float value = 0.5f;
    bool dragging = false;
};


