#include "StatusPanel.h"
#include "../simulation/SimulationEngine.h"
#include "../systems/PowerSystem.h"
#include <algorithm>

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

void StatusPanel::drawPower(Rectangle r, float& uiPowerLevel) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKBLUE, 0.3f));
    DrawText("Power", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);
    
    DrawText(TextFormat("Battery: %.0f%%", s.batteryPercent), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText("Weapons Power", (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
    
    // Interactive iOS-style slider
    Rectangle sliderRect = { r.x + 200, r.y + 60, 120, 40 };
    bool powerOn = (uiPowerLevel > 0.5f);
    
    // Draw slider background (grey when OFF, green when ON)
    Color sliderColor = powerOn ? GREEN : DARKGRAY;
    DrawRectangleRec(sliderRect, sliderColor);
    DrawRectangleLinesEx(sliderRect, 2, powerOn ? RAYWHITE : GRAY);
    
    // Draw slider handle (slides left/right)
    float handleX = powerOn ? (sliderRect.x + sliderRect.width - 20) : (sliderRect.x + 2);
    Rectangle handleRect = { handleX, sliderRect.y + 2, 16, sliderRect.height - 4 };
    DrawRectangleRec(handleRect, RAYWHITE);
    DrawRectangleLinesEx(handleRect, 1, BLACK);
    
    // Draw ON/OFF text
    const char* buttonText = powerOn ? "ON" : "OFF";
    int textWidth = MeasureText(buttonText, 16);
    int textX = (int)(sliderRect.x + (sliderRect.width - textWidth) / 2);
    int textY = (int)(sliderRect.y + (sliderRect.height - 16) / 2);
    DrawText(buttonText, textX, textY, 16, powerOn ? RAYWHITE : LIGHTGRAY);
    
    // Handle mouse interaction
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, sliderRect)) {
        // Toggle power state
        uiPowerLevel = powerOn ? 0.0f : 1.0f;
        if (power) power->setPowerLevel(uiPowerLevel);
    }
    
    DrawText("Click to toggle", (int)sliderRect.x, (int)(sliderRect.y + sliderRect.height + 5), 14, LIGHTGRAY);
}

void StatusPanel::drawDepth(Rectangle r) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKGREEN, 0.3f));
    DrawText("Depth", (int)r.x + 10, (int)r.y + 8, 20, LIME);
    DrawText(TextFormat("Depth: %.1fm", s.submarineDepthMeters), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Optimal: %.1fm", s.optimalLaunchDepthMeters), (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
}
