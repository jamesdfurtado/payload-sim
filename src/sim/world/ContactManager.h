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



    
private:
    std::vector<SonarContact> activeContacts;
    uint32_t nextContactId = 1;
    float spawnTimer = 0.0f;

};


