#pragma once

#include <raylib.h>
#include "../simulation/SimulationEngine.h"

class PowerSystem : public ISimSystem {
public:
    const char* getName() const override { return "PowerSystem"; }
    void update(SimulationState& state, float dt) override;

    // Player interactions
    void setPowerLevel(float amount01);
    float getPowerLevel() const { return weaponsPower01; }

private:
    float baseDrainPerSec = 0.5f;
    float weaponsPower01 = 0.0f; // 0..1 allocation to weapons/launch (starts OFF)
};


