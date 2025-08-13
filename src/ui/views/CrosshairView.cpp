#include "CrosshairView.h"

void CrosshairView::drawOnSonar(const Rectangle& sonarBounds) const {
    // Draw selection circle around mouse cursor if hovering over sonar
    if (crosshairManager.isMouseOverSonar()) {
        Vector2 mousePos = crosshairManager.getMousePosition();
        drawSelectionCircle(mousePos, sonarBounds);
    }
    
    // Draw tracking crosshair if tracking a contact
    if (crosshairManager.isTracking()) {
        Vector2 crosshairWorldPos = crosshairManager.getCrosshairPosition();
        drawCrosshair(crosshairWorldPos, sonarBounds);
    }
}

Vector2 CrosshairView::worldToScreen(Vector2 world, const Rectangle& sonarBounds) const {
    // Convert world coordinates to screen coordinates
    float nx = (world.x + 600.0f) / 1200.0f;
    float ny = (world.y + 360.0f) / 720.0f;
    
    return { sonarBounds.x + nx * sonarBounds.width, sonarBounds.y + ny * sonarBounds.height };
}

void CrosshairView::drawCrosshair(Vector2 position, const Rectangle& sonarBounds) const {
    Vector2 screenPos = worldToScreen(position, sonarBounds);
    
    // Snap to nearest pixel to eliminate jiggling
    int x = (int)(screenPos.x + 0.5f);
    int y = (int)(screenPos.y + 0.5f);
    
    // Draw yellow crosshair as a circle with a "+" inside
    Color crosshairColor = YELLOW;
    int radius = 12;  // Same size as cursor circle
    int thickness = 1; // Same thickness as circle lines
    
    // Draw the circle
    DrawCircleLines(x, y, radius, crosshairColor);
    
    // Draw the "+" inside the circle with longer lines that extend beyond
    int plusSize = 15; // Longer lines that go outside the circle
    DrawLineEx(Vector2{(float)(x - plusSize), (float)y}, 
               Vector2{(float)(x + plusSize), (float)y}, thickness, crosshairColor);
    DrawLineEx(Vector2{(float)x, (float)(y - plusSize)}, 
               Vector2{(float)x, (float)(y + plusSize)}, thickness, crosshairColor);
}

void CrosshairView::drawSelectionCircle(Vector2 position, const Rectangle& sonarBounds) const {
    // Draw selection circle around mouse cursor (no coordinate conversion needed)
    Color circleColor = Fade(YELLOW, 0.3f);
    DrawCircleLines((int)position.x, (int)position.y, 12, circleColor);
}
