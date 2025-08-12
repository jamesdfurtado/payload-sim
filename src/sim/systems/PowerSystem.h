#pragma once

#include "../ISystem.h"

class PowerSystem : public ISystem {
public:
    const char* getName() const override { return "PowerSystem"; }
    void update(SimulationState& state, float /*dt*/) override {
        state.powerLevel = desiredPower;
        state.powerSupplyStable = state.powerLevel > 0.1f;
    }

    void setPowerLevel(float level) { desiredPower = level; }
    float getPowerLevel() const { return desiredPower; }

private:
    float desiredPower = 0.0f;
};


