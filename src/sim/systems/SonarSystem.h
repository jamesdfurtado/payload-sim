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
        contactManager.removeOutOfBoundsContacts();

        // Maintain a selected target if available; fall back to nearest
        if (selectedTargetId != 0 && !contactManager.isContactAlive(selectedTargetId)) {
            selectedTargetId = 0;
        }
        if (selectedTargetId == 0) {
            selectedTargetId = contactManager.getNearestContactId({0,0});
        }
        // Note: targetAcquired is now managed exclusively by TargetAcquisitionSystem
        // to prevent race conditions and flickering
    }

    void attemptManualLock(const Vector2& worldPos) {
        selectedTargetId = contactManager.getNearestContactId(worldPos, 40.0f);
    }

    uint32_t getSelectedTargetId() const { return selectedTargetId; }

private:
    ContactManager& contactManager;
    uint32_t selectedTargetId = 0;
};


