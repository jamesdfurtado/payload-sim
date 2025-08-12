#pragma once

#include "../Widget.h"
#include <functional>
#include <string>

class Button : public Widget {
public:
    explicit Button(std::string label, std::function<void()> onClick)
        : label(std::move(label)), onClick(std::move(onClick)) {}

    void draw() const override {
        DrawRectangleRec(bounds, DARKGRAY);
        DrawRectangleLinesEx(bounds, 2, RAYWHITE);
        int tw = MeasureText(label.c_str(), 18);
        DrawText(label.c_str(), (int)(bounds.x + (bounds.width - tw)/2), (int)(bounds.y + bounds.height/2 - 9), 18, RAYWHITE);
    }

    bool onMouseUp(Vector2 pos) override {
        if (contains(pos) && onClick) { onClick(); return true; }
        return false;
    }

private:
    std::string label;
    std::function<void()> onClick;
};


