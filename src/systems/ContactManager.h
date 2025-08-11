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

struct MissileState {
    bool active = false;
    Vector2 position{0,0};
    Vector2 velocity{0,0};
    Vector2 target{0,0};
    std::vector<Vector2> trail;
    float progress = 0.0f;
    float explosionTimer = 0.0f;
    uint32_t targetId = 0;         // Target contact ID
    bool targetValid = true;        // Track if target is still valid during flight
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
    
    // Missile management
    void launchMissile(uint32_t targetId, const Vector2& launchPosition);
    void updateMissile(float dt);
    const MissileState& getMissileState() const { return missileState; }
    bool isMissileActive() const { return missileState.active; }
    
    // Utility
    size_t getContactCount() const { return activeContacts.size(); }
    bool hasContact(uint32_t id) const;

private:
    std::vector<SonarContact> activeContacts;
    uint32_t nextContactId;
    MissileState missileState;
    
    // Missile constants
    static constexpr float MISSILE_SPEED = 300.0f;  // Increased from 100.0f for 3x faster movement
    static constexpr float EXPLOSION_DURATION = 0.4f;  // Reduced from 0.6f for quicker explosion
    static constexpr float MISSILE_COLLISION_DISTANCE = 16.0f;
    
    // Helper methods
    void adjustTargetIndicesAfterRemoval(size_t removedIndex);
    static float calculateDistance(const Vector2& a, const Vector2& b);
    static float rand01();
    
    // Missile helper methods
    void updateMissileFlight(float dt);
    void updateMissileExplosion(float dt);
    void handleMissileCollision();
    bool isTargetInRange(uint32_t targetId, const Vector2& position, float maxDistance = 100.0f);
};
