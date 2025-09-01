#pragma once

#include "../Widget.h"
#include "../widgets/Throttle.h"
#include "../../sim/SimulationEngine.h"
#include "../../sim/systems/DepthSystem.h"
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>

class DepthView : public Widget {
public:
    DepthView(SimulationEngine& engine, DepthSystem& depth) 
        : engine(engine), depth(depth) {
        
        // depth control slider
        depthThrottle = std::make_unique<Throttle>([this](float value) {
            this->depth.setThrottleValue(value);
        });
    }

    void draw() const override {
        const auto& s = engine.getState();
        
        // depth panel
        DrawRectangleRec(bounds, Fade(DARKGREEN, 0.3f));
        DrawText("Depth", (int)bounds.x + 10, (int)bounds.y + 8, 20, LIME);
        
        // current depth display
        DrawText("Current:", (int)bounds.x + 10, (int)bounds.y + 40, 18, RAYWHITE);
        Color currentDepthColor = s.depthClearanceMet ? GREEN : RED;
        std::string currentDepthStr = formatFloat(s.currentDepthMeters, 1) + "m";
        DrawText(currentDepthStr.c_str(),
                 (int)bounds.x + 100, (int)bounds.y + 40, 18, currentDepthColor);

        // target depth label
        DrawText("Optimal:", (int)bounds.x + 10, (int)bounds.y + 64, 18, RAYWHITE);
        std::string optimalDepthStr = formatFloat(depth.getOptimalDepth(), 1) + "m";
        DrawText(optimalDepthStr.c_str(),
                 (int)bounds.x + 100, (int)bounds.y + 64, 18, SKYBLUE);
        
        // throttle positioning
        float throttleWidth = 50;
        float throttleX = bounds.x + (bounds.width - throttleWidth) / 2 - 70;
        Rectangle throttleRect = { throttleX, bounds.y + 12, throttleWidth, bounds.height - 25 };
        depthThrottle->setBounds(throttleRect);
        
        depthThrottle->draw();
        
        // movement status display
        const char* direction = depth.getMovementStatus();
        
        float statusX = throttleX + throttleWidth + 200;
        DrawText(direction, (int)statusX, (int)bounds.y + 40, 18, RAYWHITE);
        std::string throttleStr = "Throttle: " + std::to_string((int)depth.getThrottlePercentage()) + "%";
        DrawText(throttleStr.c_str(), 
                (int)statusX, (int)bounds.y + 65, 16, LIGHTGRAY);
    }

    bool onMouseDown(Vector2 pos) override {
        return depthThrottle->onMouseDown(pos);
    }

    bool onMouseUp(Vector2 pos) override {
        return depthThrottle->onMouseUp(pos);
    }

    void update(float dt) override {
        depthThrottle->update(dt);
    }

private:
    std::string formatFloat(float value, int precision) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }

    SimulationEngine& engine;
    DepthSystem& depth;
    std::unique_ptr<Throttle> depthThrottle;
};
