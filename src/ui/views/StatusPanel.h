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
        
        // Display all available status indicators
        light("Authorization", s.canLaunchAuthorized);
        light("Target Validated", s.targetValidated);
        light("Target Acquired", s.targetAcquired);
        light("Depth OK", s.depthClearanceMet);
        light("Tube Integrity", s.launchTubeIntegrity);
        light("Payload OK", s.payloadSystemOperational);
        light("No Friendlies", s.noFriendlyUnitsInBlastRadius);
        light("Conditions OK", s.launchConditionsFavorable);
        light("Power Stable", s.powerSupplyStable);
    }

private:
    SimulationEngine& engine;
};


