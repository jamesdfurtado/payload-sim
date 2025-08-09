#include "EnvironmentSystem.h"
#include <raylib.h>
#include <algorithm>

void EnvironmentSystem::update(SimulationState& state, float dt) {
    timer -= dt;
    if (timer <= 0.0f) {
        // Random walk
        seaState01 = std::clamp(seaState01 + (GetRandomValue(-10, 10) / 100.0f), 0.0f, 1.0f);
        timer = 2.0f;
    }

    state.launchConditionsFavorable = seaState01 < 0.7f;
}


