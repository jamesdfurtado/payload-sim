#include "ContactManager.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <raylib.h>

#ifndef PI
#define PI 3.14159265359f
#endif

static float rand01() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

ContactManager::ContactManager() {
    spawnTimer = 1.5f;
}

uint32_t ContactManager::spawnContact() {
    SonarContact c{};
    c.id = nextContactId++;
    
    // Reset counter every 128 contacts to prevent overflow
    if (nextContactId > 128) {
        nextContactId = 1;
    }
    
    // Random position within bounds
    c.position = { (float)GetRandomValue(-500, 500), (float)GetRandomValue(-300, 300) };
    
    // Random direction and speed
    c.velocityDirRad = rand01() * 2.0f * PI;
    c.speed = 10.0f + rand01() * 20.0f;  // Speed range: 10-30 units
    
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
        if (r < 0.25f) c.type = ContactType::EnemySub;          // 25%
        else if (r < 0.45f) c.type = ContactType::FriendlySub;  // next 20%
        else if (r < 0.90f) c.type = ContactType::Fish;         // next 45%
        else c.type = ContactType::Debris;                      // last 10%
    }
    
    activeContacts.push_back(c);
    return c.id;
}

void ContactManager::removeContact(uint32_t id) {
    activeContacts.erase(std::remove_if(activeContacts.begin(), activeContacts.end(), [id](const SonarContact& c){return c.id==id;}), activeContacts.end());
}

void ContactManager::clearAllContacts() { activeContacts.clear(); }

uint32_t ContactManager::getNearestContactId(Vector2 position, float maxDistance) const {
    uint32_t bestId = 0;
    float bestDist2 = maxDistance * maxDistance;
    for (const auto& c : activeContacts) {
        const float dx = c.position.x - position.x;
        const float dy = c.position.y - position.y;
        const float d2 = dx*dx + dy*dy;
        if (d2 < bestDist2) { bestDist2 = d2; bestId = c.id; }
    }
    return bestId;
}

bool ContactManager::isContactAlive(uint32_t id) const {
    for (const auto& c : activeContacts) if (c.id == id) return true; return false;
}

void ContactManager::updateContactPositions(float dt) {
    for (auto& contact : activeContacts) {
        contact.position.x += cosf(contact.velocityDirRad) * contact.speed * dt;
        contact.position.y += sinf(contact.velocityDirRad) * contact.speed * dt;
    }
}

void ContactManager::updateSpawnTimer(float dt) { 
    spawnTimer -= dt; 
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

void ContactManager::launchMissile(uint32_t targetId, const Vector2& launchPosition) {
    if (!isContactAlive(targetId)) return;
    missileState = {};
    missileState.active = true;
    missileState.position = launchPosition;
    missileState.targetId = targetId;
}

void ContactManager::updateMissile(float dt) {
    if (!missileState.active) return;
    // find target
    auto it = std::find_if(activeContacts.begin(), activeContacts.end(), [&](const SonarContact& c){return c.id == missileState.targetId;});
    if (it == activeContacts.end()) {
        missileState.targetValid = false;
        missileState.active = false;
        return;
    }
    Vector2 target = it->position;
    const float dx = target.x - missileState.position.x;
    const float dy = target.y - missileState.position.y;
    const float len = std::sqrt(dx*dx + dy*dy);
    const float speed = 300.0f;
    if (len < 16.0f) {
        // hit
        removeContact(it->id);
        missileState.active = false;
        return;
    }
    missileState.position.x += (dx/len) * speed * dt;
    missileState.position.y += (dy/len) * speed * dt;
}

size_t ContactManager::getEnemyCount() const {
    size_t count = 0;
    for (const auto& c : activeContacts) {
        if (c.type == ContactType::EnemySub) count++;
    }
    return count;
}

size_t ContactManager::getFriendlyCount() const {
    size_t count = 0;
    for (const auto& c : activeContacts) {
        if (c.type == ContactType::FriendlySub) count++;
    }
    return count;
}

size_t ContactManager::getFishCount() const {
    size_t count = 0;
    for (const auto& c : activeContacts) {
        if (c.type == ContactType::Fish) count++;
    }
    return count;
}

size_t ContactManager::getDebrisCount() const {
    size_t count = 0;
    for (const auto& c : activeContacts) {
        if (c.type == ContactType::Debris) count++;
    }
    return count;
}


