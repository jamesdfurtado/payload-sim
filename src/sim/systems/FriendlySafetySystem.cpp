#include "FriendlySafetySystem.h"
#include "../SimulationState.h"
#include <cmath>

void FriendlySafetySystem::update(SimulationState& state, float dt) {
    // Only perform safety check if a target is currently acquired (locked)
    if (state.targetAcquired && crosshairManager.isTracking()) {
        // Get the position of the currently tracked contact
        Vector2 blastCenter = crosshairManager.getCrosshairPosition();
        
        // Check if there are any friendly units within the blast radius
        bool friendliesInBlastRadius = checkFriendlyUnitsInBlastRadius(blastCenter);
        
        // Update the safety flag - if friendlies are in blast radius, set to false
        state.noFriendlyUnitsInBlastRadius = !friendliesInBlastRadius;
    } else {
        // No target acquired, default to safe (true)
        state.noFriendlyUnitsInBlastRadius = true;
    }
}

bool FriendlySafetySystem::checkFriendlyUnitsInBlastRadius(Vector2 blastCenter) const {
    const std::vector<SonarContact>& contacts = contactManager.getActiveContacts();
    
    for (const auto& contact : contacts) {
        // Only check friendly submarines
        if (contact.type == ContactType::FriendlySub && contact.isVisible) {
            // Calculate distance from blast center to this friendly contact
            float dx = contact.position.x - blastCenter.x;
            float dy = contact.position.y - blastCenter.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // If friendly submarine is within blast radius, return true
            if (distance <= BLAST_RADIUS) {
                return true;
            }
        }
    }
    
    // No friendly submarines found in blast radius
    return false;
}
