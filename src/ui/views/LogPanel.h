#pragma once

#include "../Widget.h"
#include <deque>
#include <string>

class LogPanel : public Widget {
public:
    void add(const std::string& msg) {
        lines.push_back(msg);
        while (lines.size() > 8) lines.pop_front();
    }

    void draw() const override {
        DrawRectangleRec(bounds, Fade(BLACK, 0.6f));
        DrawRectangleLinesEx(bounds, 2, GRAY);
        int x = (int)bounds.x + 8;
        int y = (int)bounds.y + 8;
        DrawText("Log", x, y, 18, RAYWHITE); y += 22;
        for (const auto& s : lines) {
            DrawText(s.c_str(), x, y, 16, RAYWHITE); y += 18;
        }
    }

private:
    std::deque<std::string> lines;
};


