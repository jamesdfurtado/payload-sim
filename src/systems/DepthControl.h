#pragma once

#include <raylib.h>
#include "../simulation/SimulationEngine.h"

class DepthControl : public ISimSystem {
public:
    const char* getName() const override { return "DepthControl"; }
    void update(SimulationState& state, float dt) override;

    // Player interactions
    void setBallastThrottle(float upDown01); // 0..1 with 0.5 neutral

private:
    float ballastThrottle01 = 0.5f; // 0.0 down, 0.5 hold, 1.0 up
    float verticalSpeedMps = 0.0f;
};


