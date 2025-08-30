#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"
#include <algorithm>
#include <iostream>

class TargetValidationSystem : public ISystem {
public:
    TargetValidationSystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "TargetValidationSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Single responsibility: Only manage targetValidated flag
        // Only check for state changes when something actually happens
        
        if (state.targetAcquired) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            
            if (contactManager.isContactAlive(trackedId)) {
                // Find the tracked contact to check its type
                const auto& contacts = contactManager.getActiveContacts();
                auto it = std::find_if(contacts.begin(), contacts.end(), 
                    [trackedId](const SonarContact& c) { return c.id == trackedId; });
                
                if (it != contacts.end()) {
                    // Target is validated only if it's an enemy submarine
                    bool newValidation = (it->type == ContactType::EnemySub);
                    
                    if (newValidation != state.targetValidated) {
                        if (newValidation) {
                            std::cout << "[TargetValidationSystem] targetValidated changed from false to true" << std::endl;
                        } else {
                            std::cout << "[TargetValidationSystem] targetValidated changed from true to false" << std::endl;
                        }
                        state.targetValidated = newValidation;
                    }
                } else if (state.targetValidated) {
                    // Contact not found but still validated - clear it
                    std::cout << "[TargetValidationSystem] targetValidated changed from true to false (contact not found)" << std::endl;
                    state.targetValidated = false;
                }
            } else if (state.targetValidated) {
                // Contact not alive but still validated - clear it
                std::cout << "[TargetValidationSystem] targetValidated changed from true to false (contact not alive)" << std::endl;
                state.targetValidated = false;
            }
        } else if (state.targetValidated) {
            // No target acquired but still validated - clear it
            std::cout << "[TargetValidationSystem] targetValidated changed from true to false (no target acquired)" << std::endl;
            state.targetValidated = false;
        }
        // If no target acquired and not validated, no change needed
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
