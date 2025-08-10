#include "StatusPanel.h"
#include "../simulation/SimulationEngine.h"
#include "../systems/PowerSystem.h"

StatusPanel::StatusPanel(SimulationEngine& engine, PowerSystem* power)
    : engine(engine), power(power) {
}

void StatusPanel::drawStatusLights(Rectangle r) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKGRAY, 0.4f));
    
    const int pad = 10;
    const int boxW = 150;
    const int boxH = 28;
    
    struct Item { const char* label; bool ok; } items[] = {
        {"Authorization", s.canLaunchAuthorized},
        {"Target Validated", s.targetValidated},
        {"Target Acquired", s.targetAcquired},
        {"Depth OK", s.depthClearanceMet},
        {"Tube Integrity", s.launchTubeIntegrity},
        {"Payload OK", s.payloadSystemOperational},
        {"Power Stable", s.powerSupplyStable},
        {"No Friendlies", s.noFriendlyUnitsInBlastRadius},
        {"Conditions OK", s.launchConditionsFavorable},
    };
    
    int x = (int)r.x + pad, y = (int)r.y + pad;
    for (auto& it : items) {
        Color c = it.ok ? GREEN : RED;
        DrawRectangleLines(x, y, boxW, boxH, LIGHTGRAY);
        DrawCircle(x + 12, y + boxH/2, 8, c);
        DrawText(it.label, x + 28, y + 6, 16, RAYWHITE);
        x += boxW + pad;
        if (x + boxW > r.x + r.width) { 
            x = (int)r.x + pad; 
            y += boxH + pad; 
        }
    }
}

void StatusPanel::drawPower(Rectangle r, float uiWeaponsPower) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKBLUE, 0.3f));
    DrawText("Power", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);
    DrawText(TextFormat("Battery: %.1f%%", s.batteryPercent), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Weapons Power: %.0f%%", uiWeaponsPower * 100.0f), (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
}

void StatusPanel::drawDepth(Rectangle r) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKGREEN, 0.3f));
    DrawText("Depth", (int)r.x + 10, (int)r.y + 8, 20, LIME);
    DrawText(TextFormat("Depth: %.1fm", s.submarineDepthMeters), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Optimal: %.1fm", s.optimalLaunchDepthMeters), (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
}
