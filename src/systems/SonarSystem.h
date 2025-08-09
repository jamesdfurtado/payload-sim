#pragma once

#include <vector>
#include <raylib.h>
#include "../simulation/SimulationEngine.h"

enum class ContactType { EnemySub, FriendlySub, Fish, Debris };

struct SonarContact {
    Vector2 position;
    float velocityDirRad;
    float speed;
    ContactType type;
    bool isVisible = true;
};

class SonarSystem : public ISimSystem {
public:
    const char* getName() const override { return "SonarSystem"; }
    void update(SimulationState& state, float dt) override;

    const std::vector<SonarContact>& getContacts() const { return contacts; }

    // Player interactions
    void attemptManualLock(const Vector2& cursorWorldPos);
    bool isTargetValidated() const { return targetValidated; }
    bool isTargetAcquired() const { return targetAcquired; }

    Vector2 getLockedTarget() const { return lockedTarget; }

    void removeContact(int idx);  // Moved to .cpp for proper target disengagement

private:
    std::vector<SonarContact> contacts;
    float spawnTimer = 0.0f;

    bool targetValidated = false;
    bool targetAcquired = false;
    int lockedTargetIdx = -1;
    Vector2 lockedTarget{0,0};

    void spawnContact();
};


