#pragma once

#include "../ISystem.h"
#include <algorithm>

class TargetingSystem : public ISystem {
public:
    const char* getName() const override { return "TargetingSystem"; }
    void update(SimulationState& state, float /*dt*/) override {
        state.targetingStability = stability;
    }

    void adjustStability(float delta) { stability = std::clamp(stability + delta, 0.0f, 1.0f); }
    float getStability() const { return stability; }

private:
    float stability = 0.5f;
};


