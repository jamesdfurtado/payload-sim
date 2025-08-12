#pragma once

#include <vector>
#include <cstdint>
#include <raylib.h>

enum class ContactType { EnemySub, FriendlySub, Fish, Debris };

struct SonarContact {
    uint32_t id;
    Vector2 position;
    float velocityDirRad;
    float speed;
    ContactType type;
    bool isVisible = true;
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

    // Missile
    void launchMissile(uint32_t targetId, const Vector2& launchPosition);
    void updateMissile(float dt);
    const MissileState& getMissileState() const { return missileState; }

private:
    std::vector<SonarContact> activeContacts;
    uint32_t nextContactId = 1;
    float spawnTimer = 0.0f;
    MissileState missileState{};
};


