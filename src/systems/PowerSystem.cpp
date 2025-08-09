#include "PowerSystem.h"

void PowerSystem::update(SimulationState& state, float dt) {
    // TEMPORARY: Keep battery at 100% for testing
    state.batteryPercent = 100.0f;
    state.powerSupplyStable = true;
}

void PowerSystem::routePowerToWeapons(float amount01) {
    if (amount01 < 0.0f) amount01 = 0.0f;
    if (amount01 > 1.0f) amount01 = 1.0f;
    weaponsPower01 = amount01;
}


