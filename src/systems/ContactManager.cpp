#include "ContactManager.h"
#include <cmath>
#include <algorithm>

ContactManager::ContactManager() : nextContactId(1) {
}

uint32_t ContactManager::spawnContact() {
    SonarContact c{};
    c.id = nextContactId++;
    
    // Reset counter every 128 contacts to prevent overflow
    if (nextContactId > 128) {
        nextContactId = 1;
    }
    
    c.position = { (float)GetRandomValue(-500, 500), (float)GetRandomValue(-300, 300) };
    c.velocityDirRad = rand01() * 2.0f * PI;
    c.speed = 10.0f + rand01() * 20.0f;
    
    // Bias spawn toward enemies and ensure there is always at least one enemy
    bool hasEnemyAlready = false;
    for (const auto& existing : activeContacts) {
        if (existing.type == ContactType::EnemySub) {
            hasEnemyAlready = true;
            break;
        }
    }

    if (!hasEnemyAlready) {
        c.type = ContactType::EnemySub;
    } else {
        // Heavier weight for enemies to increase engagement frequency
        float r = rand01();
        if (r < 0.50f) c.type = ContactType::EnemySub;          // 50%
        else if (r < 0.70f) c.type = ContactType::FriendlySub;  // next 20%
        else if (r < 0.95f) c.type = ContactType::Fish;         // next 25%
        else c.type = ContactType::Debris;                      // last 5%
    }
    
    activeContacts.push_back(c);
    return c.id;
}

void ContactManager::removeContact(uint32_t id) {
    for (auto it = activeContacts.begin(); it != activeContacts.end(); ++it) {
        if (it->id == id) {
            activeContacts.erase(it);
            // Notify target tracking system if callback is set
            if (targetTrackingCallback) {
                targetTrackingCallback(id);
            }
            break;
        }
    }
}

void ContactManager::removeContactByIndex(size_t index) {
    if (index < activeContacts.size()) {
        activeContacts.erase(activeContacts.begin() + index);
    }
}

void ContactManager::clearAllContacts() {
    activeContacts.clear();
}

SonarContact* ContactManager::findContactById(uint32_t id) {
    for (auto& contact : activeContacts) {
        if (contact.id == id) {
            return &contact;
        }
    }
    return nullptr;
}

const SonarContact* ContactManager::findContactById(uint32_t id) const {
    for (const auto& contact : activeContacts) {
        if (contact.id == id) {
            return &contact;
        }
    }
    return nullptr;
}

std::vector<uint32_t> ContactManager::findContactsInRadius(Vector2 center, float radius, ContactType type) {
    std::vector<uint32_t> foundContacts;
    
    for (const auto& contact : activeContacts) {
        if (contact.type == type) {
            float distance = calculateDistance(contact.position, center);
            if (distance < radius) {
                foundContacts.push_back(contact.id);
            }
        }
    }
    
    return foundContacts;
}

void ContactManager::updateContactPositions(float dt) {
    for (auto& contact : activeContacts) {
        contact.position.x += cosf(contact.velocityDirRad) * contact.speed * dt;
        contact.position.y += sinf(contact.velocityDirRad) * contact.speed * dt;
    }
}

void ContactManager::removeOutOfBoundsContacts() {
    for (auto it = activeContacts.begin(); it != activeContacts.end(); ) {
        if (it->position.x < -600 || it->position.x > 600 || 
            it->position.y < -360 || it->position.y > 360) {
            it = activeContacts.erase(it);
        } else {
            ++it;
        }
    }
}

bool ContactManager::isContactAlive(uint32_t id) const {
    return findContactById(id) != nullptr;
}

uint32_t ContactManager::getNearestContactId(Vector2 position, float maxDistance) const {
    float nearest = maxDistance;
    uint32_t bestId = 0;
    
    for (const auto& contact : activeContacts) {
        float distance = calculateDistance(contact.position, position);
        if (distance < nearest) {
            nearest = distance;
            bestId = contact.id;
        }
    }
    
    return bestId;
}

bool ContactManager::hasContact(uint32_t id) const {
    return findContactById(id) != nullptr;
}

// Missile Management Methods
void ContactManager::launchMissile(uint32_t targetId, const Vector2& launchPosition) {
    if (!hasContact(targetId)) {
        return; // Can't launch if target doesn't exist
    }
    
    missileState.active = true;
    missileState.trail.clear();
    missileState.position = launchPosition;
    missileState.targetId = targetId;
    missileState.targetValid = true;
    missileState.explosionTimer = 0.0f;
    missileState.progress = 0.0f;
    
    // Set initial target position
    const SonarContact* target = findContactById(targetId);
    if (target) {
        missileState.target = target->position;
    }
    
    // Add initial position to trail for smooth animation
    missileState.trail.push_back(launchPosition);
}

void ContactManager::updateMissile(float dt) {
    if (!missileState.active) {
        return;
    }
    
    // Check if target still exists
    if (missileState.targetId != 0 && !hasContact(missileState.targetId)) {
        // Target was destroyed, explode immediately
        missileState.explosionTimer = EXPLOSION_DURATION;
        missileState.targetValid = false;
    }
    
    if (missileState.explosionTimer > 0.0f) {
        updateMissileExplosion(dt);
    } else {
        updateMissileFlight(dt);
    }
}

void ContactManager::updateMissileFlight(float dt) {
    // Update target position if target still exists
    if (missileState.targetId != 0 && hasContact(missileState.targetId)) {
        const SonarContact* target = findContactById(missileState.targetId);
        if (target) {
            missileState.target = target->position;
            missileState.targetValid = true;
        }
    } else {
        missileState.targetValid = false;
    }
    
    // Calculate missile movement
    Vector2 toTarget = {missileState.target.x - missileState.position.x, missileState.target.y - missileState.position.y};
    float dist = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    
    if (dist > 1.0f) {
        // Move missile towards target
        Vector2 velocity = {toTarget.x / dist * MISSILE_SPEED * dt, toTarget.y / dist * MISSILE_SPEED * dt};
        missileState.position.x += velocity.x;
        missileState.position.y += velocity.y;
        missileState.velocity = velocity;
        
        // Update trail more frequently for smooth animation
        missileState.trail.push_back(missileState.position);
        if (missileState.trail.size() > 60) {  // Increased from 40 to 60 for smoother trail
            missileState.trail.erase(missileState.trail.begin());
        }
        
        // Check for collision
        if (dist < MISSILE_COLLISION_DISTANCE) {
            handleMissileCollision();
        }
    } else {
        // Missile reached target
        missileState.position = missileState.target;
        handleMissileCollision();
    }
}

void ContactManager::updateMissileExplosion(float dt) {
    missileState.explosionTimer -= dt;
    
    if (missileState.explosionTimer <= 0.0f) {
        // Explosion complete, destroy target if it still exists
        if (missileState.targetValid && missileState.targetId != 0 && hasContact(missileState.targetId)) {
            removeContact(missileState.targetId);
        }
        
        // Reset missile state
        missileState.active = false;
        missileState.explosionTimer = 0.0f;
        missileState.targetValid = false;
        missileState.targetId = 0;
        missileState.trail.clear();
    }
}

void ContactManager::handleMissileCollision() {
    // Start explosion phase
    missileState.explosionTimer = EXPLOSION_DURATION;
    
    // If target is still valid, mark it for destruction
    if (missileState.targetValid && missileState.targetId != 0 && hasContact(missileState.targetId)) {
        // Target will be destroyed when explosion completes
    }
}

bool ContactManager::isTargetInRange(uint32_t targetId, const Vector2& position, float maxDistance) {
    const SonarContact* target = findContactById(targetId);
    if (!target) {
        return false;
    }
    
    float distance = calculateDistance(target->position, position);
    return distance < maxDistance;
}

void ContactManager::adjustTargetIndicesAfterRemoval(size_t /* removedIndex */) {
    // This method is kept for compatibility but shouldn't be needed with ID-based system
    // Left empty as we're moving away from index-based tracking
}

float ContactManager::calculateDistance(const Vector2& a, const Vector2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

float ContactManager::rand01() {
    return (float)GetRandomValue(0, 10000) / 10000.0f;
}

void ContactManager::spawnContactsIfNeeded() {
    // Ensure board is populated at all times
    while (activeContacts.size() < 10) {
        spawnContact();
    }
    
    // If there are no enemies on the board, force-spawn one (up to the cap)
    {
        bool enemyPresent = false;
        for (const auto& c : activeContacts) {
            if (c.type == ContactType::EnemySub) { enemyPresent = true; break; }
        }
        if (!enemyPresent && activeContacts.size() < 20) {
            spawnContact();
        }
    }

    if (spawnTimer <= 0.0f && activeContacts.size() < 20) {
        spawnContact();
        spawnTimer = 1.5f + ((float)GetRandomValue(0, 10000) / 10000.0f) * 2.0f;
    }
}

void ContactManager::updateSpawnTimer(float dt) {
    spawnTimer -= dt;
}

bool ContactManager::launchMissileAtTarget(uint32_t targetId, const Vector2& launchPosition) {
    if (!hasContact(targetId)) {
        return false; // Can't launch if target doesn't exist
    }
    
    launchMissile(targetId, launchPosition);
    return true;
}
