#pragma once

#include <raylib.h>
#include "../simulation/SimulationEngine.h"
#include "ContactManager.h"

class SonarSystem : public ISimSystem {
public:
    SonarSystem(ContactManager& contactManager);
    const char* getName() const override { return "SonarSystem"; }
    void update(SimulationState& state, float dt) override;

    const std::vector<SonarContact>& getContacts() const { return contactManager.getActiveContacts(); }

    // Player interactions
    void attemptManualLock(const Vector2& cursorWorldPos);
    bool isTargetValidated() const { return targetValidated; }
    bool isTargetAcquired() const { return targetAcquired; }

    Vector2 getLockedTarget() const { return lockedTarget; }

    // Target tracking
    uint32_t getLockedTargetId() const { return lockedTargetId; }

    // Missile state access
    const MissileState& getMissileState() const;

private:
    ContactManager& contactManager;  // Reference to shared ContactManager

    bool targetValidated = false;
    bool targetAcquired = false;
    uint32_t lockedTargetId = 0;
    Vector2 lockedTarget{0,0};
};


