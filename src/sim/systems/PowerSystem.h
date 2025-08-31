#pragma once

#include "../ISystem.h"

class PowerSystem : public ISystem {
public:
    const char* getName() const override { return "PowerSystem"; }
    
    // handles battery drain/charge based on power switch
    void update(SimulationState& state, float dt) override {
        if (desiredPower > 0.5f) {
            batteryLevel -= batteryDrainRate * dt;
            if (batteryLevel < 0.0f) batteryLevel = 0.0f;
        } else {
            batteryLevel += batteryChargeRate * dt;
            if (batteryLevel > 100.0f) batteryLevel = 100.0f;
        }
        
        state.powerLevel = batteryLevel / 100.0f;
        state.powerSupplyStable = (desiredPower > 0.5f) && (batteryLevel > 0.0f);
    }

    void setPowerLevel(float level) { desiredPower = level; }
    void setPowerState(bool isOn) { desiredPower = isOn ? 1.0f : 0.0f; }
    float getPowerLevel() const { return desiredPower; }
    float getBatteryLevel() const { return batteryLevel; }

private:
    float desiredPower = 0.0f;
    float batteryLevel = 100.0f;
    
    const float batteryDrainRate = 4.0f;
    const float batteryChargeRate = 15.0f;
};


