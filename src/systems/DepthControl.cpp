#include "DepthControl.h"
#include <cmath>

void DepthControl::update(SimulationState& state, float dt) {
    float thrust = (ballastThrottle01 - 0.5f) * 2.0f; // -1..1
    verticalSpeedMps += thrust * 12.0f * dt; // much stronger acceleration
    verticalSpeedMps *= 0.92f; // slightly less damping for faster movement

    // Positive vertical speed means ascending (reducing depth)
    state.submarineDepthMeters -= verticalSpeedMps * dt;
    if (state.submarineDepthMeters < 5.0f) state.submarineDepthMeters = 5.0f;
    if (state.submarineDepthMeters > 400.0f) state.submarineDepthMeters = 400.0f;

    float depthError = fabsf(state.submarineDepthMeters - state.optimalLaunchDepthMeters);
    state.depthClearanceMet = depthError < 5.0f; // within 5 meters
}

void DepthControl::setBallastThrottle(float upDown01) {
    if (upDown01 < 0.0f) upDown01 = 0.0f;
    if (upDown01 > 1.0f) upDown01 = 1.0f;
    ballastThrottle01 = upDown01;
}


