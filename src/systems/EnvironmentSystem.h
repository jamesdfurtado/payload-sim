#pragma once

#include "../simulation/SimulationEngine.h"

class EnvironmentSystem : public ISimSystem {
public:
    const char* getName() const override { return "EnvironmentSystem"; }
    void update(SimulationState& state, float dt) override;

private:
    float timer = 0.0f;
    float seaState01 = 0.2f; // 0 calm .. 1 storm
};


