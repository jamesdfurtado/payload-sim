#pragma once

#include "../Widget.h"
#include "../widgets/Switch.h"
#include "../../sim/SimulationEngine.h"
#include "../../sim/systems/PowerSystem.h"
#include <memory>
#include <string>

class PowerView : public Widget {
public:
    PowerView(SimulationEngine& engine, PowerSystem& power) 
        : engine(engine), power(power) {
        
        // Create the weapons power switch
        weaponsSwitch = std::make_unique<Switch>(false, [this](bool state) {
            // Convert boolean to power level (0.0f = OFF, 1.0f = ON)
            float powerLevel = state ? 1.0f : 0.0f;
            this->power.setPowerLevel(powerLevel);
        });
    }

    void draw() const override {
        const auto& s = engine.getState();
        
        // Draw blue background panel
        DrawRectangleRec(bounds, Fade(DARKBLUE, 0.3f));
        
        // Draw title
        DrawText("Power", (int)bounds.x + 10, (int)bounds.y + 8, 20, SKYBLUE);
        
        // Draw battery status
        int batteryPercent = (int)power.getBatteryLevel();
        DrawText(("Battery: " + std::to_string(batteryPercent) + "%").c_str(), 
                (int)bounds.x + 10, (int)bounds.y + 40, 18, RAYWHITE);
        
        // Draw weapons power label
        DrawText("Weapons Power", (int)bounds.x + 10, (int)bounds.y + 64, 18, RAYWHITE);
        
        // Position and draw the switch
        Rectangle switchRect = { bounds.x + 200, bounds.y + 35, 120, 40 };
        weaponsSwitch->setBounds(switchRect);
        weaponsSwitch->draw();
        
        // Draw instruction text
        DrawText("Click to toggle", (int)switchRect.x, (int)(switchRect.y + switchRect.height + 5), 
                14, LIGHTGRAY);
    }

    bool onMouseDown(Vector2 pos) override {
        return weaponsSwitch->onMouseDown(pos);
    }

    bool onMouseUp(Vector2 pos) override {
        return weaponsSwitch->onMouseUp(pos);
    }

private:
    SimulationEngine& engine;
    PowerSystem& power;
    std::unique_ptr<Switch> weaponsSwitch;
};
