#include "FriendlySafetySystem.h"
#include "../SimulationState.h"
#include <cmath>

void FriendlySafetySystem::update(SimulationState& state, float dt) {
    // use friendly blast radius check
    if (state.targetAcquired && crosshairManager.isTracking()) {
        Vector2 blastCenter = crosshairManager.getCrosshairPosition();
        bool friendliesInBlastRadius = checkFriendlyUnitsInBlastRadius(blastCenter);
        state.noFriendlyUnitsInBlastRadius = !friendliesInBlastRadius;
    } else {
        state.noFriendlyUnitsInBlastRadius = true;
    }
}


// check if friendly submarines are in blast radius
bool FriendlySafetySystem::checkFriendlyUnitsInBlastRadius(Vector2 blastCenter) const {
    const std::vector<SonarContact>& contacts = contactManager.getActiveContacts();
    
    float dx = 0.0f - blastCenter.x;
    float dy = 0.0f - blastCenter.y;
    float distanceToCenter = std::sqrt(dx * dx + dy * dy);
    if (distanceToCenter <= BLAST_RADIUS) {
        return true;
    }
    
    // calcuate blast radius range
    for (const auto& contact : contacts) {
        if (contact.type == ContactType::FriendlySub && contact.isVisible) {
            float dx = contact.position.x - blastCenter.x;
            float dy = contact.position.y - blastCenter.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= BLAST_RADIUS) {
                return true;
            }
        }
    }
    
    return false;
}
