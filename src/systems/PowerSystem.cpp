#include "PowerSystem.h"

void PowerSystem::update(SimulationState& state, float dt) {
    float weaponsDrain = 2.0f * weaponsPower01; // more allocation drains faster
    float otherDrain = 1.0f * (1.0f - weaponsPower01);
    float totalDrain = baseDrainPerSec + weaponsDrain + otherDrain;

    state.batteryPercent -= totalDrain * dt;
    if (state.batteryPercent < 0.0f) state.batteryPercent = 0.0f;
    if (state.batteryPercent > 100.0f) state.batteryPercent = 100.0f;

    state.powerSupplyStable = (state.batteryPercent > 10.0f);
}

void PowerSystem::routePowerToWeapons(float amount01) {
    if (amount01 < 0.0f) amount01 = 0.0f;
    if (amount01 > 1.0f) amount01 = 1.0f;
    weaponsPower01 = amount01;
}


