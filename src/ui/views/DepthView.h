#pragma once

#include "../Widget.h"
#include "../widgets/Throttle.h"
#include "../../sim/SimulationEngine.h"
#include "../../sim/systems/DepthSystem.h"
#include <memory>

class DepthView : public Widget {
public:
    DepthView(SimulationEngine& engine, DepthSystem& depth) 
        : engine(engine), depth(depth) {
        
        // Create the depth throttle
        depthThrottle = std::make_unique<Throttle>([this](float value) {
            // Convert throttle value (0.0f = DOWN, 0.5f = NEUTRAL, 1.0f = UP) to depth change
            float depthChange = (value - 0.5f) * 1.0f; // -0.5f to 0.5f (halved sensitivity)
            this->depth.setDepthChange(depthChange);
        });
    }

    void draw() const override {
        const auto& s = engine.getState();
        
        // Draw dark green background panel
        DrawRectangleRec(bounds, Fade(DARKGREEN, 0.3f));
        
        // Draw title
        DrawText("Depth", (int)bounds.x + 10, (int)bounds.y + 8, 20, LIME);
        
        // Draw current depth label
        DrawText("Current:", (int)bounds.x + 10, (int)bounds.y + 40, 18, RAYWHITE);
        // Draw current depth value - red if outside range, green if within range
        Color currentDepthColor = s.depthClearanceMet ? GREEN : RED;
        DrawText(TextFormat("%.1fm", s.currentDepthMeters),
                 (int)bounds.x + 100, (int)bounds.y + 40, 18, currentDepthColor);

        // Draw optimal depth label
        DrawText("Optimal:", (int)bounds.x + 10, (int)bounds.y + 64, 18, RAYWHITE);
        // Draw optimal depth value in cyan (desired/target color)
        DrawText(TextFormat("%.1fm", depth.getOptimalDepth()),
                 (int)bounds.x + 100, (int)bounds.y + 64, 18, SKYBLUE);
        
        // Position and draw the throttle
        float throttleWidth = 50;
        float throttleX = bounds.x + (bounds.width - throttleWidth) / 2 - 70; // Center horizontally, then left 70px
        Rectangle throttleRect = { throttleX, bounds.y + 12, throttleWidth, bounds.height - 25 }; // Moved up 10px (35-10=25)
        depthThrottle->setBounds(throttleRect);
        
        depthThrottle->draw();
        
        // Draw status and throttle value (positioned to the right of the throttle, moved 100px right)
        const char* direction = "";
        float throttleValue = depthThrottle->getValue();
        if (throttleValue > 0.55f) direction = "ASCENDING";
        else if (throttleValue < 0.45f) direction = "DESCENDING";
        else direction = "HOLDING";
        
        float statusX = throttleX + throttleWidth + 200;
        DrawText(direction, (int)statusX, (int)bounds.y + 40, 18, RAYWHITE);
        DrawText(TextFormat("Throttle: %.0f%%", (throttleValue - 0.5f) * 200.0f), 
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
    SimulationEngine& engine;
    DepthSystem& depth;
    std::unique_ptr<Throttle> depthThrottle;
};
