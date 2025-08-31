#include "MissileSystem.h"
#include <algorithm>
#include <iostream>

// main missile system update loop - handles launches, targeting, and explosions
void MissileSystem::update(SimulationState& state, float dt) {
    if (state.missileLaunched && !state.missileActive) {
        triggerLaunch(state);
    }
    
    // if target is lost, explode the missile
    if (state.missileActive) {
        uint32_t trackedContactId = crosshairManager.getTrackedContactId();
        
        if (trackedContactId == 0) {
            std::cout << "[MissileSystem] Target lost (trackedContactId == 0), exploding missile" << std::endl;
            missileManager.explodeAllMissiles();
            state.missileActive = false;
            state.missileTargetId = 0;
        }
        else if (!contactManager.isContactAlive(trackedContactId)) {
            std::cout << "[MissileSystem] Tracked target no longer alive, exploding missile" << std::endl;
            missileManager.explodeAllMissiles();
            state.missileActive = false;
            state.missileTargetId = 0;
        }
        else {
            // find tracked target ID and update missile guidance
            const auto& contacts = contactManager.getActiveContacts();
            for (size_t i = 0; i < contacts.size(); ++i) {
                if (contacts[i].id == trackedContactId) {
                    missileManager.updateMissileTargets(static_cast<uint32_t>(i));
                    break;
                }
            }
        }
    }
    
    std::vector<Vector2> targetPositions = getTargetPositions();
    missileManager.updateMissilePhysics(dt, targetPositions);
    
    missileManager.updateExplosions(dt);
    
    // collision detection
    std::vector<uint32_t> hitContactIds;
    std::vector<Vector2> contactPositions;
    
    for (const auto& contact : contactManager.getActiveContacts()) {
        contactPositions.push_back(contact.position);
    }
    
    missileManager.checkCollisions(contactPositions, hitContactIds);
    
    if (!hitContactIds.empty()) {
        handleExplosions(hitContactIds);
    }
    
    // update simulation state based on current missile/explosion status
    state.missileActive = !missileManager.getActiveMissiles().empty();
    state.explosionActive = !missileManager.getActiveExplosions().empty();
    
    if (state.missileActive) {
        state.missileLaunched = false;
    }
}

// fires a missile if we have a valid target locked
void MissileSystem::triggerLaunch(SimulationState& state) {
    if (!state.targetAcquired) {
        return;
    }
    
    uint32_t trackedContactId = crosshairManager.getTrackedContactId();
    if (trackedContactId == 0) {
        return;
    }
    
    if (!contactManager.isContactAlive(trackedContactId)) {
        return;
    }
    
    // find the target's index in our active contacts
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
    
    uint32_t missileId = missileManager.launchMissile({0, 0}, targetIndex);
    
    if (missileId != 0) {
        state.missileTargetId = trackedContactId;
        state.missileActive = true;
    }
}

// getss all contact positions for missile guidance
std::vector<Vector2> MissileSystem::getTargetPositions() const {
    std::vector<Vector2> positions;
    
    for (const auto& contact : contactManager.getActiveContacts()) {
        positions.push_back(contact.position);
    }
    
    return positions;
}

// removes contacts that got blown up by missiles
void MissileSystem::handleExplosions(const std::vector<uint32_t>& hitContactIds) {
    for (uint32_t contactIndex : hitContactIds) {
        const auto& contacts = contactManager.getActiveContacts();
        if (contactIndex < contacts.size()) {
            uint32_t contactId = contacts[contactIndex].id;
            contactManager.removeContact(contactId);
        }
    }
}
