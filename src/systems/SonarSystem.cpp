#include "SonarSystem.h"
#include <cmath>

SonarSystem::SonarSystem(ContactManager& contactManager) : contactManager(contactManager) {
    // Set up callback for when contacts are removed
    contactManager.setTargetTrackingCallback([this](uint32_t removedId) {
        if (removedId == lockedTargetId) {
            // If removing the currently locked target, disengage crosshair
            targetAcquired = false;
            targetValidated = false;
            lockedTargetId = 0;
        }
    });
}

void SonarSystem::update(SimulationState& state, float dt) {
    // Update spawn timer and spawn contacts if needed
    contactManager.updateSpawnTimer(dt);
    contactManager.spawnContactsIfNeeded();
    
    // Update contact positions
    contactManager.updateContactPositions(dt);
    
    // Remove out of bounds contacts
    contactManager.removeOutOfBoundsContacts();
    
    // Update missile if active
    contactManager.updateMissile(dt);
    
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
        float dx = lockedTarget.x;
        float dy = lockedTarget.y;
        float distance = sqrtf(dx * dx + dy * dy);
        if (distance < blastRadius) {
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

const MissileState& SonarSystem::getMissileState() const {
    return contactManager.getMissileState();
}


