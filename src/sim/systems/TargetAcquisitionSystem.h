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
    
    // manages the targetAcquired condition if crosshair is tracking
    void update(SimulationState& state, float dt) override {
        bool currentlyTracking = crosshairManager.isTracking();
        
        if (currentlyTracking) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            bool contactAlive = contactManager.isContactAlive(trackedId);
            
            if (contactAlive && !state.targetAcquired) {
                state.targetAcquired = true;
            } else if (!contactAlive && state.targetAcquired) {
                state.targetAcquired = false;
            }
        } else if (state.targetAcquired) {
            state.targetAcquired = false;
        }
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
