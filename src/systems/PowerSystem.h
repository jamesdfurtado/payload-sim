#pragma once

#include <raylib.h>
#include "../simulation/SimulationEngine.h"

class PowerSystem : public ISimSystem {
public:
    const char* getName() const override { return "PowerSystem"; }
    void update(SimulationState& state, float dt) override;

    // Player interactions
    void routePowerToWeapons(float amount01);
    float getWeaponsPower() const { return weaponsPower01; }

private:
    float baseDrainPerSec = 0.5f;
    float weaponsPower01 = 0.5f; // 0..1 allocation to weapons/launch
};


