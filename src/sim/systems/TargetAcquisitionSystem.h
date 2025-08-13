#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"
#include <algorithm>

class TargetAcquisitionSystem : public ISystem {
public:
    TargetAcquisitionSystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "TargetAcquisitionSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Update target acquisition status based on crosshair tracking
        if (crosshairManager.isTracking()) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            
            // Check if the tracked contact still exists and is visible
            if (contactManager.isContactAlive(trackedId)) {
                // Target is acquired
                state.targetAcquired = true;
                
                // Check if target is validated (enemy contact)
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
                // Contact no longer exists, clear target status
                state.targetAcquired = false;
                state.targetValidated = false;
            }
        } else {
            // No target being tracked, clear target status
            state.targetAcquired = false;
            state.targetValidated = false;
        }
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
