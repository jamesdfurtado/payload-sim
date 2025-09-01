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
        
        // weapons power toggle
        weaponsSwitch = std::make_unique<Switch>(false, [this](bool state) {
            this->power.setPowerState(state);
        });
    }

    void update(float dt) override {
        bool powerSystemState = (power.getPowerLevel() > 0.5f);
        if (weaponsSwitch->getState() != powerSystemState) {
            weaponsSwitch->setStateQuiet(powerSystemState); // silent update
        }
    }

    void draw() const override {
        const auto& s = engine.getState();
        
        // draw power panel
        DrawRectangleRec(bounds, Fade(DARKBLUE, 0.3f));
        DrawText("Power", (int)bounds.x + 10, (int)bounds.y + 8, 20, SKYBLUE);
        
        // battery percentage
        int batteryPercent = (int)power.getBatteryLevel();
        DrawText(("Battery: " + std::to_string(batteryPercent) + "%").c_str(), 
                (int)bounds.x + 10, (int)bounds.y + 40, 18, RAYWHITE);
        
        DrawText("Weapons Power", (int)bounds.x + 10, (int)bounds.y + 64, 18, RAYWHITE);
        
        Rectangle switchRect = { bounds.x + 200, bounds.y + 35, 120, 40 };
        weaponsSwitch->setBounds(switchRect);
        weaponsSwitch->draw();
        
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
