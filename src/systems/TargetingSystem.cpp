#include "TargetingSystem.h"

void TargetingSystem::update(SimulationState& state, float dt) {
    // Natural decay
    stability01 -= 0.05f * dt;
    if (stability01 < 0.0f) stability01 = 0.0f;

    // Target acquisition harder if unstable
    if (!state.targetAcquired) {
        // If validated but not acquired, require stability threshold to allow lock
        if (state.targetValidated && stability01 > 0.6f) {
            // leave acquisition to SonarSystem manual lock, this system just influences UI
        }
    }
}

void TargetingSystem::adjustStability(float delta01) {
    stability01 += delta01;
    if (stability01 < 0.0f) stability01 = 0.0f;
    if (stability01 > 1.0f) stability01 = 1.0f;
}


