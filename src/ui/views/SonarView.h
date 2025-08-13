#pragma once

#include "../Widget.h"
#include "../../sim/world/ContactManager.h"

class SonarView : public Widget {
public:
    explicit SonarView(ContactManager& contacts) : contacts(contacts) {}

    void draw() const override {
        // Draw panel, grid, crosshair, and submarine icon
        drawSonar(bounds, contacts.getMissileState());
        
        // Note: Contact rendering is handled by ContactView, not here
    }

    bool onMouseUp(Vector2 pos) override {
        if (!contains(pos)) return false;
        float nx = (pos.x - bounds.x) / bounds.width;
        float ny = (pos.y - bounds.y) / bounds.height;
        Vector2 world{ nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
        // Selection will now be handled by SonarSystem via attemptManualLock
        if (onSelect) onSelect(world);
        return true;
    }

    // Allow the root to connect selection to the sonar system
    void setOnSelect(std::function<void(Vector2)> fn) { onSelect = std::move(fn); }

    // Get the current bounds for contact overlay rendering
    const Rectangle& getBounds() const { return bounds; }

private:
    // Coordinate transform: world (-600..600, -360..360) to screen inside rectangle
    static Vector2 worldToScreen(Vector2 world, const Rectangle& r) {
        float nx = (world.x + 600.0f) / 1200.0f;
        float ny = (world.y + 360.0f) / 720.0f;
        return { r.x + nx * r.width, r.y + ny * r.height };
    }

    void drawSonar(Rectangle r, const MissileState& /*missileState*/) const {
        DrawRectangleRec(r, Fade(BLUE, 0.2f));
        DrawRectangleLinesEx(r, 1, SKYBLUE);
        DrawText("Sonar", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);

        drawSonarGrid(r);
        drawCrossHair(r);
        drawSubmarineIcon(r);
    }

    void drawSubmarineIcon(Rectangle r) const {
        Vector2 subCenter = worldToScreen({0,0}, r);

        int bodyWidth = 24;
        int bodyHeight = 14;
        int bodyX = static_cast<int>(subCenter.x) - static_cast<int>(bodyWidth/2);
        int bodyY = static_cast<int>(subCenter.y) - static_cast<int>(bodyHeight/2);

        DrawRectangle(bodyX + 2, bodyY, bodyWidth - 4, bodyHeight, DARKGRAY);
        DrawCircle(bodyX + 2, static_cast<int>(subCenter.y), static_cast<int>(bodyHeight/2), DARKGRAY);
        DrawCircle(bodyX + bodyWidth - 2, static_cast<int>(subCenter.y), static_cast<int>(bodyHeight/2), DARKGRAY);

        int towerWidth = 6;
        int towerHeight = 6;
        int towerX = bodyX + 4;
        int towerY = bodyY - towerHeight;

        DrawRectangle(towerX, towerY + 2, towerWidth, towerHeight - 2, GRAY);
        DrawCircle(static_cast<int>(towerX + towerWidth/2), towerY + 2, static_cast<int>(towerWidth/2), GRAY);
        DrawCircle(static_cast<int>(towerX + towerWidth/2), towerY, 2, DARKGRAY);
    }

    void drawSonarGrid(Rectangle r) const {
        Color gridColor = Fade(SKYBLUE, 0.15f);
        Vector2 center = { r.x + r.width/2.0f, r.y + r.height/2.0f };
        int maxRadius = static_cast<int>(static_cast<float>(std::min(r.width, r.height)) / 2.0f);

        BeginScissorMode(static_cast<int>(r.x), static_cast<int>(r.y), static_cast<int>(r.width), static_cast<int>(r.height));
        for (int radius = 50; radius <= maxRadius + 100; radius += 50) {
            DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, gridColor);
        }
        EndScissorMode();
    }

    void drawCrossHair(Rectangle r) const {
        Color gridColor = Fade(SKYBLUE, 0.15f);
        Vector2 center = { r.x + r.width/2.0f, r.y + r.height/2.0f };
        DrawLine(static_cast<int>(center.x), static_cast<int>(r.y), static_cast<int>(center.x), static_cast<int>(r.y + r.height), gridColor);
        DrawLine(static_cast<int>(r.x), static_cast<int>(center.y), static_cast<int>(r.x + r.width), static_cast<int>(center.y), gridColor);
        DrawLine(static_cast<int>(r.x), static_cast<int>(r.y), static_cast<int>(r.x + r.width), static_cast<int>(r.y + r.height), gridColor);
        DrawLine(static_cast<int>(r.x + r.width), static_cast<int>(r.y), static_cast<int>(r.x), static_cast<int>(r.y + r.height), gridColor);
    }

    ContactManager& contacts;
    std::function<void(Vector2)> onSelect;
};


