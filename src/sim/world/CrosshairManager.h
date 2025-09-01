#pragma once

#include <cstdint>
#include <raylib.h>
#include "ContactManager.h"

class CrosshairManager {
public:
    CrosshairManager(ContactManager& contacts) : contactManager(contacts) {}

    void update(float dt);
    void updateMousePosition(Vector2 mousePos, const Rectangle& sonarBounds);
    
    // returns true if a contact was selected
    bool handleMouseClick(Vector2 mousePos, const Rectangle& sonarBounds);
    
    bool isTracking() const { return trackedContactId != 0; }
    uint32_t getTrackedContactId() const { return trackedContactId; }
    Vector2 getCrosshairPosition() const { return crosshairPosition; }
    Vector2 getMousePosition() const { return mousePosition; }
    bool isMouseOverSonar() const { return mouseOverSonar; }
    
    void clearTracking() { trackedContactId = 0; }

private:
    ContactManager& contactManager;
    
    uint32_t trackedContactId = 0;
    Vector2 crosshairPosition = {0, 0};
    
    Vector2 mousePosition = {0, 0};
    bool mouseOverSonar = false;
    
    static constexpr float SELECTION_RADIUS = 20.0f;
    Vector2 screenToWorld(Vector2 screenPos, const Rectangle& sonarBounds) const;
    bool isContactInSelectionCircle(Vector2 contactWorldPos, Vector2 mouseWorldPos) const;
};
