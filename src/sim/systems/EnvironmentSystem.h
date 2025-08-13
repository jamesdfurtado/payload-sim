#pragma once

#include "../ISystem.h"

class EnvironmentSystem : public ISystem {
public:
    const char* getName() const override { return "EnvironmentSystem"; }
    void update(SimulationState& state, float /*dt*/) override {
        // Keep it simple for now
        state.launchConditionsFavorable = true;
        state.launchTubeIntegrity = true;

    }
};


