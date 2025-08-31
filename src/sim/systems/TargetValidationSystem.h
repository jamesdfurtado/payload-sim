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
    
    // check if acquired target is an enemy sub or not
    void update(SimulationState& state, float dt) override {
        if (state.targetAcquired) {
            uint32_t trackedId = crosshairManager.getTrackedContactId();
            
            if (contactManager.isContactAlive(trackedId)) {
                // find the contact in the contact list
                const auto& contacts = contactManager.getActiveContacts();
                auto it = std::find_if(contacts.begin(), contacts.end(), 
                    [trackedId](const SonarContact& c) { return c.id == trackedId; });
                
                if (it != contacts.end()) {
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
                    std::cout << "[TargetValidationSystem] targetValidated changed from true to false (contact not found)" << std::endl;
                    state.targetValidated = false;
                }
            } else if (state.targetValidated) {
                std::cout << "[TargetValidationSystem] targetValidated changed from true to false (contact not alive)" << std::endl;
                state.targetValidated = false;
            }
        } else if (state.targetValidated) {
            std::cout << "[TargetValidationSystem] targetValidated changed from true to false (no target acquired)" << std::endl;
            state.targetValidated = false;
        }
    }

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
};
