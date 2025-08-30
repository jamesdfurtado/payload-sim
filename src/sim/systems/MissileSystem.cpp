#include "MissileSystem.h"
#include <algorithm>
#include <iostream>

void MissileSystem::update(SimulationState& state, float dt) {
    // Handle missile launch trigger
    if (state.missileLaunched && !state.missileActive) {
        triggerLaunch(state);
    }
    
    // Check if missile should explode due to lost target
    if (state.missileActive) {
        uint32_t trackedContactId = crosshairManager.getTrackedContactId();
        
        // If no contact is being tracked (trackedContactId == 0), explode the missile
        if (trackedContactId == 0) {
            std::cout << "[MissileSystem] Target lost (trackedContactId == 0), exploding missile" << std::endl;
            missileManager.explodeAllMissiles();
            state.missileActive = false;
            state.missileTargetId = 0;
        }
        // If tracked contact is no longer alive, explode the missile
        else if (!contactManager.isContactAlive(trackedContactId)) {
            std::cout << "[MissileSystem] Tracked target no longer alive, exploding missile" << std::endl;
            missileManager.explodeAllMissiles();
            state.missileActive = false;
            state.missileTargetId = 0;
        }
        // Target is valid, update missile targeting
        else {
            // Find the target index in the contacts list
            const auto& contacts = contactManager.getActiveContacts();
            for (size_t i = 0; i < contacts.size(); ++i) {
                if (contacts[i].id == trackedContactId) {
                    missileManager.updateMissileTargets(static_cast<uint32_t>(i));
                    break;
                }
            }
        }
    }
    
    // Update missile physics with target positions
    std::vector<Vector2> targetPositions = getTargetPositions();
    missileManager.updateMissilePhysics(dt, targetPositions);
    
    // Update explosions
    missileManager.updateExplosions(dt);
    
    // Check for collisions and handle explosions
    std::vector<uint32_t> hitContactIds;
    std::vector<Vector2> contactPositions;
    
    // Get all contact positions for collision detection
    for (const auto& contact : contactManager.getActiveContacts()) {
        contactPositions.push_back(contact.position);
    }
    
    missileManager.checkCollisions(contactPositions, hitContactIds);
    
    // Handle any explosions
    if (!hitContactIds.empty()) {
        handleExplosions(hitContactIds);
    }
    
    // Update state flags
    state.missileActive = !missileManager.getActiveMissiles().empty();
    state.explosionActive = !missileManager.getActiveExplosions().empty();
    
    // Reset launch flag after successful launch
    if (state.missileActive) {
        state.missileLaunched = false;
    }
}

void MissileSystem::triggerLaunch(SimulationState& state) {
    // Check if we have a target acquired
    if (!state.targetAcquired) {
        return;
    }
    
    // Get the target from crosshair manager
    uint32_t trackedContactId = crosshairManager.getTrackedContactId();
    if (trackedContactId == 0) {
        return;
    }
    
    // Check if target is still alive
    if (!contactManager.isContactAlive(trackedContactId)) {
        return;
    }
    
    // Find the target index in the contacts list
    const auto& contacts = contactManager.getActiveContacts();
    uint32_t targetIndex = 0;
    bool foundTarget = false;
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        if (contacts[i].id == trackedContactId) {
            targetIndex = static_cast<uint32_t>(i);
            foundTarget = true;
            break;
        }
    }
    
    if (!foundTarget) {
        std::cout << "[MissileSystem] Target not found in contacts list" << std::endl;
        return;
    }
    
    // Launch missile from submarine position (0,0)
    uint32_t missileId = missileManager.launchMissile({0, 0}, targetIndex);
    
    if (missileId != 0) {
        state.missileTargetId = trackedContactId;
        state.missileActive = true;
    }
}

std::vector<Vector2> MissileSystem::getTargetPositions() const {
    std::vector<Vector2> positions;
    
    // Get positions of all contacts for missile targeting
    for (const auto& contact : contactManager.getActiveContacts()) {
        positions.push_back(contact.position);
    }
    
    return positions;
}

void MissileSystem::handleExplosions(const std::vector<uint32_t>& hitContactIds) {
    // Remove contacts that were hit by missiles
    for (uint32_t contactIndex : hitContactIds) {
        // Find the contact by index in the active contacts list
        const auto& contacts = contactManager.getActiveContacts();
        if (contactIndex < contacts.size()) {
            uint32_t contactId = contacts[contactIndex].id;
            contactManager.removeContact(contactId);
            
            // The crosshair manager will handle clearing its trackedContactId when the contact is removed
        }
    }
}
