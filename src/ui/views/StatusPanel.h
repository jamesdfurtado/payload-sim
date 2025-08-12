#pragma once

#include "../Widget.h"
#include "../../sim/SimulationEngine.h"

class StatusPanel : public Widget {
public:
    explicit StatusPanel(SimulationEngine& engine) : engine(engine) {}

    void draw() const override {
        const auto& s = engine.getState();
        DrawRectangleRec(bounds, Fade(DARKBLUE, 0.5f));
        DrawRectangleLinesEx(bounds, 2, SKYBLUE);
        int x = (int)bounds.x + 10;
        int y = (int)bounds.y + 8;
        DrawText("Status", x, y, 20, RAYWHITE);
        y += 28;
        auto light = [&](const char* name, bool ok){ DrawText(name, x, y, 18, ok? LIME: RED); y += 22; };
        light("Power", s.powerSupplyStable);
        light("Depth Clearance", s.depthClearanceMet);
        light("Target Acquired", s.targetAcquired);
        light("Target Validated", s.targetValidated);
        light("Payload Ready", s.payloadSystemOperational);
        light("Auth OK", s.canLaunchAuthorized);
    }

private:
    SimulationEngine& engine;
};


