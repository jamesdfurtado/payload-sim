#pragma once

#include "../ISystem.h"

class DepthSystem : public ISystem {
public:
    const char* getName() const override { return "DepthSystem"; }
    void update(SimulationState& state, float /*dt*/) override {
        state.currentDepthMeters = desiredDepth;
        state.depthClearanceMet = state.currentDepthMeters >= 50.0f && state.currentDepthMeters <= 300.0f;
    }

    void setDepth(float meters) { desiredDepth = meters; }
    float getDepth() const { return desiredDepth; }

private:
    float desiredDepth = 100.0f;
};


