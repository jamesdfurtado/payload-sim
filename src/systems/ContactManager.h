#pragma once

#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "../simulation/SimulationEngine.h"

enum class ContactType { EnemySub, FriendlySub, Fish, Debris };

struct SonarContact {
    uint32_t id;                    // Unique identifier (never changes)
    Vector2 position;               // Current position
    float velocityDirRad;           // Direction of movement
    float speed;                    // Speed of movement
    ContactType type;               // Type of contact
    bool isVisible = true;          // Visibility flag
};

class ContactManager {
public:
    ContactManager();
    
    // Contact lifecycle management
    uint32_t spawnContact();
    void removeContact(uint32_t id);
    void removeContactByIndex(size_t index);
    void clearAllContacts();
    
    // Contact querying
    const std::vector<SonarContact>& getActiveContacts() const { return activeContacts; }
    SonarContact* findContactById(uint32_t id);
    const SonarContact* findContactById(uint32_t id) const;
    std::vector<uint32_t> findContactsInRadius(Vector2 center, float radius, ContactType type = ContactType::EnemySub);
    
    // Contact updates
    void updateContactPositions(float dt);
    void removeOutOfBoundsContacts();
    
    // Target tracking
    bool isContactAlive(uint32_t id) const;
    uint32_t getNearestContactId(Vector2 position, float maxDistance = 25.0f) const;
    
    // Utility
    size_t getContactCount() const { return activeContacts.size(); }
    bool hasContact(uint32_t id) const;

private:
    std::vector<SonarContact> activeContacts;
    uint32_t nextContactId;
    
    // Helper methods
    void adjustTargetIndicesAfterRemoval(size_t removedIndex);
    static float calculateDistance(const Vector2& a, const Vector2& b);
    static float rand01();
};
