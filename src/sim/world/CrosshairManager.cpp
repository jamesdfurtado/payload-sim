#include "CrosshairManager.h"
#include <algorithm>

// keeps crosshair locked onto tracked contact
void CrosshairManager::update(float dt) {
    if (trackedContactId != 0) {
        const auto& contacts = contactManager.getActiveContacts();
        auto it = std::find_if(contacts.begin(), contacts.end(), 
            [this](const SonarContact& c) { return c.id == trackedContactId; });
        
        if (it != contacts.end()) {
            crosshairPosition = it->position;
        } else {
            trackedContactId = 0;
        }
    }
}

void CrosshairManager::updateMousePosition(Vector2 mousePos, const Rectangle& sonarBounds) {
    mousePosition = mousePos;
    
    mouseOverSonar = (mousePos.x >= sonarBounds.x && mousePos.x <= sonarBounds.x + sonarBounds.width &&
                      mousePos.y >= sonarBounds.y && mousePos.y <= sonarBounds.y + sonarBounds.height);
}

// click on target = start tracking
bool CrosshairManager::handleMouseClick(Vector2 mousePos, const Rectangle& sonarBounds) {
    if (!mouseOverSonar) return false;
    
    Vector2 mouseWorldPos = screenToWorld(mousePos, sonarBounds);
    
    // check if clicked on contact
    const auto& contacts = contactManager.getActiveContacts();
    for (const auto& contact : contacts) {
        if (isContactInSelectionCircle(contact.position, mouseWorldPos)) {
            trackedContactId = contact.id;
            crosshairPosition = contact.position;
            return true;
        }
    }
    
    trackedContactId = 0;
    return false;
}

Vector2 CrosshairManager::screenToWorld(Vector2 screenPos, const Rectangle& sonarBounds) const {
    float nx = (screenPos.x - sonarBounds.x) / sonarBounds.width;
    float ny = (screenPos.y - sonarBounds.y) / sonarBounds.height;
    
    return { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
}

bool CrosshairManager::isContactInSelectionCircle(Vector2 contactWorldPos, Vector2 mouseWorldPos) const {
    float dx = contactWorldPos.x - mouseWorldPos.x;
    float dy = contactWorldPos.y - mouseWorldPos.y;
    float distanceSquared = dx * dx + dy * dy;
    
    return distanceSquared <= SELECTION_RADIUS * SELECTION_RADIUS;
}
