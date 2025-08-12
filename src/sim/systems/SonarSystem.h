#pragma once

#include "../ISystem.h"
#include "../SimulationState.h"
#include "../world/ContactManager.h"

class SonarSystem : public ISystem {
public:
    explicit SonarSystem(ContactManager& contacts) : contactManager(contacts) {}
    const char* getName() const override { return "SonarSystem"; }

    void update(SimulationState& state, float dt) override {
        contactManager.updateContactPositions(dt);
        contactManager.updateSpawnTimer(dt);
        contactManager.spawnContactsIfNeeded();
        contactManager.updateMissile(dt);
        state.targetAcquired = contactManager.getNearestContactId({0,0}) != 0; // simplistic
    }

    void attemptManualLock(const Vector2& worldPos) {
        (void)worldPos; // placeholder: could lock nearest contact
    }

private:
    ContactManager& contactManager;
};


