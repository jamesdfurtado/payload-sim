#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"

class TargetAcquisitionSystem : public ISystem {
public:
    TargetAcquisitionSystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "TargetAcquisitionSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Single responsibility: Only manage targetAcquired flag
        if (crosshairManager.isTracking()) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            
            // Check if the tracked contact still exists and is visible
            if (contactManager.isContactAlive(trackedId)) {
                // Target is acquired
                state.targetAcquired = true;
            } else {
                // Contact no longer exists, clear target status
                state.targetAcquired = false;
            }
        } else {
            // No target being tracked, clear target status
            state.targetAcquired = false;
        }
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
