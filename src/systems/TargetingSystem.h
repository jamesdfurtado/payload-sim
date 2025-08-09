#pragma once

#include "../simulation/SimulationEngine.h"

class TargetingSystem : public ISimSystem {
public:
    const char* getName() const override { return "TargetingSystem"; }
    void update(SimulationState& state, float dt) override;

    // Player can stabilize targeting reticle
    void adjustStability(float delta01);

private:
    float stability01 = 0.5f; // 0..1
};


