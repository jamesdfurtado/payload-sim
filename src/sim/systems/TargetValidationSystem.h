#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"
#include <algorithm>

class TargetValidationSystem : public ISystem {
public:
    TargetValidationSystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "TargetValidationSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Single responsibility: Only manage targetValidated flag
        if (state.targetAcquired) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            
            // Check if the tracked contact still exists
            if (contactManager.isContactAlive(trackedId)) {
                // Find the tracked contact to check its type
                const auto& contacts = contactManager.getActiveContacts();
                auto it = std::find_if(contacts.begin(), contacts.end(), 
                    [trackedId](const SonarContact& c) { return c.id == trackedId; });
                
                if (it != contacts.end()) {
                    // Target is validated only if it's an enemy submarine
                    state.targetValidated = (it->type == ContactType::EnemySub);
                } else {
                    state.targetValidated = false;
                }
            } else {
                state.targetValidated = false;
            }
        } else {
            // No target acquired, clear validation
            state.targetValidated = false;
        }
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
