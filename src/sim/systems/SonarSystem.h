#pragma once

#include "../ISystem.h"
#include "../SimulationState.h"
#include "../world/ContactManager.h"

class SonarSystem : public ISystem {
public:
    explicit SonarSystem(ContactManager& contacts) : contactManager(contacts) {}
    const char* getName() const override { return "SonarSystem"; }

    // manages contact movement, spawning, and target selection
    void update(SimulationState& state, float dt) override {
        contactManager.updateContactPositions(dt);
        contactManager.updateSpawnTimer(dt);
        contactManager.spawnContactsIfNeeded();
        contactManager.removeOutOfBoundsContacts();

        
        // Ensure the selected target is valid
        if (selectedTargetId != 0 && !contactManager.isContactAlive(selectedTargetId)) {
            selectedTargetId = 0;
        }
        if (selectedTargetId == 0) {
            selectedTargetId = contactManager.getNearestContactId({0,0});
        }
    }

    void attemptManualLock(const Vector2& worldPos) {
        selectedTargetId = contactManager.getNearestContactId(worldPos, 40.0f);
    }

    uint32_t getSelectedTargetId() const { return selectedTargetId; }

private:
    ContactManager& contactManager;
    uint32_t selectedTargetId = 0;
};


