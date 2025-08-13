#pragma once

#include <cstdint>
#include <raylib.h>
#include "ContactManager.h"

class CrosshairManager {
public:
    CrosshairManager(ContactManager& contacts) : contactManager(contacts) {}

    // Update crosshair position based on tracked contact
    void update(float dt);
    
    // Handle mouse hover and selection circle
    void updateMousePosition(Vector2 mousePos, const Rectangle& sonarBounds);
    
    // Handle mouse click to select/track a contact
    bool handleMouseClick(Vector2 mousePos, const Rectangle& sonarBounds);
    
    // Get current crosshair state
    bool isTracking() const { return trackedContactId != 0; }
    uint32_t getTrackedContactId() const { return trackedContactId; }
    Vector2 getCrosshairPosition() const { return crosshairPosition; }
    Vector2 getMousePosition() const { return mousePosition; }
    bool isMouseOverSonar() const { return mouseOverSonar; }
    
    // Clear tracking
    void clearTracking() { trackedContactId = 0; }

private:
    ContactManager& contactManager;
    
    // Tracking state
    uint32_t trackedContactId = 0;
    Vector2 crosshairPosition = {0, 0};
    
    // Mouse state
    Vector2 mousePosition = {0, 0};
    bool mouseOverSonar = false;
    
    // Selection circle parameters
    static constexpr float SELECTION_RADIUS = 20.0f;
    
    // Helper methods
    Vector2 screenToWorld(Vector2 screenPos, const Rectangle& sonarBounds) const;
    bool isContactInSelectionCircle(Vector2 contactWorldPos, Vector2 mouseWorldPos) const;
};
