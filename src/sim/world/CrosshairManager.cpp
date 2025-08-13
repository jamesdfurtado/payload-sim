#include "CrosshairManager.h"
#include <algorithm>

void CrosshairManager::update(float dt) {
    // Update crosshair position if tracking a contact
    if (trackedContactId != 0) {
        const auto& contacts = contactManager.getActiveContacts();
        auto it = std::find_if(contacts.begin(), contacts.end(), 
            [this](const SonarContact& c) { return c.id == trackedContactId; });
        
        if (it != contacts.end()) {
            // Update crosshair position to match tracked contact
            crosshairPosition = it->position;
        } else {
            // Contact no longer exists, clear tracking
            trackedContactId = 0;
        }
    }
}

void CrosshairManager::updateMousePosition(Vector2 mousePos, const Rectangle& sonarBounds) {
    mousePosition = mousePos;
    
    // Check if mouse is over sonar view
    mouseOverSonar = (mousePos.x >= sonarBounds.x && mousePos.x <= sonarBounds.x + sonarBounds.width &&
                      mousePos.y >= sonarBounds.y && mousePos.y <= sonarBounds.y + sonarBounds.height);
}

bool CrosshairManager::handleMouseClick(Vector2 mousePos, const Rectangle& sonarBounds) {
    if (!mouseOverSonar) return false;
    
    Vector2 mouseWorldPos = screenToWorld(mousePos, sonarBounds);
    
    // Check if click is within selection circle of any contact
    const auto& contacts = contactManager.getActiveContacts();
    for (const auto& contact : contacts) {
        if (isContactInSelectionCircle(contact.position, mouseWorldPos)) {
            // Start tracking this contact
            trackedContactId = contact.id;
            crosshairPosition = contact.position;
            return true;
        }
    }
    
    // No contact selected, clear tracking
    trackedContactId = 0;
    return false;
}

Vector2 CrosshairManager::screenToWorld(Vector2 screenPos, const Rectangle& sonarBounds) const {
    // Convert screen coordinates to world coordinates
    // First, get relative position within sonar bounds (0.0 to 1.0)
    float nx = (screenPos.x - sonarBounds.x) / sonarBounds.width;
    float ny = (screenPos.y - sonarBounds.y) / sonarBounds.height;
    
    // Transform to world space (-600..600, -360..360)
    return { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
}

bool CrosshairManager::isContactInSelectionCircle(Vector2 contactWorldPos, Vector2 mouseWorldPos) const {
    // Check if contact is within selection radius of mouse
    float dx = contactWorldPos.x - mouseWorldPos.x;
    float dy = contactWorldPos.y - mouseWorldPos.y;
    float distanceSquared = dx * dx + dy * dy;
    
    return distanceSquared <= SELECTION_RADIUS * SELECTION_RADIUS;
}
