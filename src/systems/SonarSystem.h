#pragma once

#include <raylib.h>
#include "../simulation/SimulationEngine.h"
#include "ContactManager.h"

class SonarSystem : public ISimSystem {
public:
    const char* getName() const override { return "SonarSystem"; }
    void update(SimulationState& state, float dt) override;

    const std::vector<SonarContact>& getContacts() const { return contactManager.getActiveContacts(); }

    // Player interactions
    void attemptManualLock(const Vector2& cursorWorldPos);
    bool isTargetValidated() const { return targetValidated; }
    bool isTargetAcquired() const { return targetAcquired; }

    Vector2 getLockedTarget() const { return lockedTarget; }

    // Contact management
    void removeContact(uint32_t id);
    uint32_t getLockedTargetId() const { return lockedTargetId; }

    // Missile management
    void launchMissile();
    const MissileState& getMissileState() const;

private:
    ContactManager contactManager;
    float spawnTimer = 0.0f;

    bool targetValidated = false;
    bool targetAcquired = false;
    uint32_t lockedTargetId = 0;  // Changed from index to ID
    Vector2 lockedTarget{0,0};

    void spawnContactsIfNeeded();
};


