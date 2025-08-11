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
    
    float r = rand01();
    if (r < 0.25f) c.type = ContactType::EnemySub;
    else if (r < 0.40f) c.type = ContactType::FriendlySub;
    else if (r < 0.80f) c.type = ContactType::Fish;
    else c.type = ContactType::Debris;
    
    activeContacts.push_back(c);
    return c.id;
}

void ContactManager::removeContact(uint32_t id) {
    for (auto it = activeContacts.begin(); it != activeContacts.end(); ++it) {
        if (it->id == id) {
            activeContacts.erase(it);
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
