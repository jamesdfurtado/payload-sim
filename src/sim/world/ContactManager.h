#pragma once

#include <vector>
#include <cstdint>
#include <raylib.h>

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
    uint32_t targetId = 0;
    bool targetValid = true;
};

class ContactManager {
public:
    ContactManager();

    uint32_t spawnContact();
    void removeContact(uint32_t id);
    void clearAllContacts();

    const std::vector<SonarContact>& getActiveContacts() const { return activeContacts; }
    uint32_t getNearestContactId(Vector2 position, float maxDistance = 25.0f) const;
    bool isContactAlive(uint32_t id) const;

    void updateContactPositions(float dt);
    void updateSpawnTimer(float dt);
    void spawnContactsIfNeeded();
    void removeOutOfBoundsContacts();

    // Missile
    void launchMissile(uint32_t targetId, const Vector2& launchPosition);
    void updateMissile(float dt);
    const MissileState& getMissileState() const { return missileState; }

    // Contact statistics
    size_t getContactCount() const { return activeContacts.size(); }
    size_t getEnemyCount() const;
    size_t getFriendlyCount() const;
    size_t getFishCount() const;
    size_t getDebrisCount() const;

private:
    std::vector<SonarContact> activeContacts;
    uint32_t nextContactId = 1;
    float spawnTimer = 0.0f;
    MissileState missileState{};
};


