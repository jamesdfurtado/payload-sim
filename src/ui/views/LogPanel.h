#pragma once

#include "../Widget.h"
#include <deque>
#include <string>

class LogPanel : public Widget {
public:
    void setPayloadPhase(const std::string& phase) {
        currentPhase = phase;
    }

    void addLogMessage(const std::string& msg) {
        logLines.push_back(msg);
        while (logLines.size() > 6) logLines.pop_front(); // Keep 6 log lines
    }

    void draw() const override {
        // Draw main panel background
        DrawRectangleRec(bounds, Fade(DARKGRAY, 0.8f));
        DrawRectangleLinesEx(bounds, 2, GRAY);
        
        int x = (int)bounds.x + 8;
        int y = (int)bounds.y + 8;
        
        // Draw title
        DrawText("SYSTEM STATUS", x, y, 16, RAYWHITE);
        y += 20;
        
        // Draw payload phase prominently
        DrawText("Payload Phase:", x, y, 14, LIGHTGRAY);
        y += 16;
        DrawText(currentPhase.c_str(), x + 4, y, 16, YELLOW);
        y += 24;
        
        // Draw separator line
        DrawLineEx(Vector2{(float)x, (float)y}, Vector2{(float)(x + bounds.width - 16), (float)y}, 1, GRAY);
        y += 8;
        
        // Draw log messages in grey
        DrawText("Activity Log:", x, y, 12, LIGHTGRAY);
        y += 14;
        
        for (const auto& msg : logLines) {
            DrawText(msg.c_str(), x + 4, y, 12, Fade(LIGHTGRAY, 0.8f));
            y += 14;
        }
        
        // Fill remaining space with placeholder if no logs
        while (y < bounds.y + bounds.height - 8) {
            DrawText("...", x + 4, y, 12, Fade(DARKGRAY, 0.5f));
            y += 14;
        }
    }

private:
    std::string currentPhase = "Idle";
    std::deque<std::string> logLines;
};
