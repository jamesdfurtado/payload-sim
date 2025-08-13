#pragma once

#include "../Widget.h"
#include "../../sim/world/CrosshairManager.h"

class CrosshairView : public Widget {
public:
    explicit CrosshairView(CrosshairManager& crosshair) : crosshairManager(crosshair) {}

    void draw() const override {
        // This view doesn't draw its own panel - it only renders crosshair elements
    }

    // Draw crosshair and selection circle on the sonar display
    void drawOnSonar(const Rectangle& sonarBounds) const;

private:
    CrosshairManager& crosshairManager;
    
    // Helper methods
    Vector2 worldToScreen(Vector2 world, const Rectangle& sonarBounds) const;
    void drawCrosshair(Vector2 position, const Rectangle& sonarBounds) const;
    void drawSelectionCircle(Vector2 position, const Rectangle& sonarBounds) const;
};
