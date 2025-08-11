#include "SonarSystem.h"
#include <cmath>

// Helper function (kept for compatibility)
static float calculateDistance(const Vector2& a, const Vector2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

void SonarSystem::update(SimulationState& state, float dt) {
    spawnTimer -= dt;
    
    // Spawn contacts if needed
    spawnContactsIfNeeded();
    
    // Update contact positions
    contactManager.updateContactPositions(dt);
    
    // Remove out of bounds contacts
    contactManager.removeOutOfBoundsContacts();
    
    // Handle target tracking
    if (targetAcquired && lockedTargetId != 0) {
        const SonarContact* targetContact = contactManager.findContactById(lockedTargetId);
        if (targetContact) {
            lockedTarget = targetContact->position;
            targetValidated = (targetContact->type == ContactType::EnemySub);
        } else {
            // Target contact was destroyed or went out of bounds
            targetAcquired = false;
            targetValidated = false;
            lockedTargetId = 0;
        }
    }

    state.targetValidated = targetValidated;
    state.targetAcquired = targetAcquired;

    // Ensure no friendlies in blast radius
    if (targetAcquired) {
        float blastRadius = 80.0f;
        bool friendlyInside = false;
        
        // Check if player submarine is in blast radius
        if (calculateDistance({0,0}, lockedTarget) < blastRadius) {
            friendlyInside = true;
        }
        
        // Check if friendly contacts are in blast radius
        if (!friendlyInside) {
            auto friendlyContacts = contactManager.findContactsInRadius(lockedTarget, blastRadius, ContactType::FriendlySub);
            friendlyInside = !friendlyContacts.empty();
        }
        
        state.noFriendlyUnitsInBlastRadius = !friendlyInside;
    }
}

void SonarSystem::attemptManualLock(const Vector2& cursorWorldPos) {
    uint32_t nearestId = contactManager.getNearestContactId(cursorWorldPos, 25.0f);
    
    if (nearestId != 0) {
        const SonarContact* contact = contactManager.findContactById(nearestId);
        if (contact) {
            lockedTargetId = nearestId;
            lockedTarget = contact->position;
            targetAcquired = true;
            targetValidated = (contact->type == ContactType::EnemySub);
        }
    }
}

void SonarSystem::removeContact(uint32_t id) {
    if (id == lockedTargetId) {
        // If removing the currently locked target, disengage crosshair
        targetAcquired = false;
        targetValidated = false;
        lockedTargetId = 0;
    }
    
    contactManager.removeContact(id);
}

void SonarSystem::spawnContactsIfNeeded() {
    // Ensure board is populated at all times
    while (contactManager.getContactCount() < 10) {
        contactManager.spawnContact();
    }
    
    if (spawnTimer <= 0.0f && contactManager.getContactCount() < 20) {
        contactManager.spawnContact();
        spawnTimer = 1.5f + ((float)GetRandomValue(0, 10000) / 10000.0f) * 2.0f;
    }
}


