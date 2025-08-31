#pragma once

#include "../ISystem.h"
#include "../SimulationState.h"
#include "../world/MissileManager.h"
#include "../world/ContactManager.h"
#include "../world/CrosshairManager.h"

class MissileSystem : public ISystem {
public:
    MissileSystem(MissileManager& missiles, ContactManager& contacts, CrosshairManager& crosshair) 
        : missileManager(missiles), contactManager(contacts), crosshairManager(crosshair) {}
    
    const char* getName() const override { return "MissileSystem"; }
    void update(SimulationState& state, float dt) override;

    // handles missile launch logic
    void triggerLaunch(SimulationState& state);
    
private:
    MissileManager& missileManager;
    ContactManager& contactManager;
    CrosshairManager& crosshairManager;
    
    std::vector<Vector2> getTargetPositions() const;
    void handleExplosions(const std::vector<uint32_t>& hitContactIds);
};
