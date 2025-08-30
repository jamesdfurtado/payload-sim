#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"
#include <iostream>

class TargetAcquisitionSystem : public ISystem {
public:
    TargetAcquisitionSystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "TargetAcquisitionSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Single responsibility: Only manage targetAcquired flag
        // Only check for state changes when something actually happens
        
        // Check if we're currently tracking
        bool currentlyTracking = crosshairManager.isTracking();
        
        if (currentlyTracking) {
            // We have a target - check if it's still valid
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            bool contactAlive = contactManager.isContactAlive(trackedId);
            
            if (contactAlive && !state.targetAcquired) {
                // Target was just acquired
                state.targetAcquired = true;
            } else if (!contactAlive && state.targetAcquired) {
                // Target was just lost
                state.targetAcquired = false;
            }
            // If both are true or both are false, no change needed
        } else if (state.targetAcquired) {
            // We're not tracking but still have targetAcquired set - clear it
            state.targetAcquired = false;
        }
        // If we're not tracking and don't have targetAcquired, no change needed
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
