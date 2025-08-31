#pragma once

#include "../ISystem.h"
#include "../world/CrosshairManager.h"
#include "../world/ContactManager.h"
#include <raylib.h>

class FriendlySafetySystem : public ISystem {
public:
    FriendlySafetySystem(CrosshairManager& crosshair, ContactManager& contacts) 
        : crosshairManager(crosshair), contactManager(contacts) {}
    
    const char* getName() const override { return "FriendlySafetySystem"; }
    void update(SimulationState& state, float dt) override;

private:
    CrosshairManager& crosshairManager;
    ContactManager& contactManager;
    
    static constexpr float BLAST_RADIUS = 35.0f;
    bool checkFriendlyUnitsInBlastRadius(Vector2 blastCenter) const;
};
