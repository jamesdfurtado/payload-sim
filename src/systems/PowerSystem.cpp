#include "PowerSystem.h"

void PowerSystem::update(SimulationState& state, float dt) {
    // Battery depletes when power is ON (>=0.5), recharges when power is OFF (<0.5)
    if (weaponsPower01 >= 0.5f) {
        // Power ON - battery depletes
        state.batteryPercent -= 5.0f * dt; // Deplete at 5% per second
        if (state.batteryPercent < 0.0f) state.batteryPercent = 0.0f;
    } else {
        // Power OFF - battery recharges
        state.batteryPercent += 8.0f * dt; // Recharge at 8% per second
        if (state.batteryPercent > 100.0f) state.batteryPercent = 100.0f;
    }
    
    // Power is stable when power is ON AND battery > 0%
    state.powerSupplyStable = (weaponsPower01 >= 0.5f && state.batteryPercent > 0.0f);
}

void PowerSystem::setPowerLevel(float amount01) {
    if (amount01 < 0.0f) amount01 = 0.0f;
    if (amount01 > 1.0f) amount01 = 1.0f;
    weaponsPower01 = amount01;
}


