#include "ContactManager.h"
#include <cmath>
#include <algorithm>
#include <random>

static float rand01() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

ContactManager::ContactManager() {
    spawnTimer = 1.0f;
}

uint32_t ContactManager::spawnContact() {
    SonarContact c{};
    c.id = nextContactId++;
    c.position = { rand01() * 1200.0f - 600.0f, rand01() * 720.0f - 360.0f };
    c.velocityDirRad = rand01() * 6.2831853f;
    c.speed = 20.0f + rand01() * 60.0f;
    c.type = ContactType::EnemySub;
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
    for (auto& c : activeContacts) {
        c.position.x += std::cos(c.velocityDirRad) * c.speed * dt;
        c.position.y += std::sin(c.velocityDirRad) * c.speed * dt;
        // wrap
        if (c.position.x < -620) c.position.x = 620;
        if (c.position.x > 620) c.position.x = -620;
        if (c.position.y < -380) c.position.y = 380;
        if (c.position.y > 380) c.position.y = -380;
    }
}

void ContactManager::updateSpawnTimer(float dt) { spawnTimer -= dt; }

void ContactManager::spawnContactsIfNeeded() {
    if (spawnTimer <= 0.0f) {
        spawnContact();
        spawnTimer = 2.0f;
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


