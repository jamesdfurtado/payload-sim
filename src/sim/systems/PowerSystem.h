#pragma once

#include "../ISystem.h"

class PowerSystem : public ISystem {
public:
    const char* getName() const override { return "PowerSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Update battery level based on power switch state
        if (desiredPower > 0.5f) {
            // Power is ON - drain battery (reduced drain rate for better efficiency)
            batteryLevel -= batteryDrainRate * dt;
            if (batteryLevel < 0.0f) batteryLevel = 0.0f;
        } else {
            // Power is OFF - charge battery
            batteryLevel += batteryChargeRate * dt;
            if (batteryLevel > 100.0f) batteryLevel = 100.0f;
        }
        
        // Update simulation state
        state.powerLevel = batteryLevel / 100.0f; // Convert percentage to 0.0-1.0 range
        state.powerSupplyStable = (desiredPower > 0.5f) && (batteryLevel > 0.0f);
    }

    void setPowerLevel(float level) { desiredPower = level; }
    void setPowerState(bool isOn) { desiredPower = isOn ? 1.0f : 0.0f; }
    float getPowerLevel() const { return desiredPower; }
    float getBatteryLevel() const { return batteryLevel; }

private:
    float desiredPower = 0.0f;  // Power switch state (0.0f = OFF, 1.0f = ON)
    float batteryLevel = 100.0f; // Battery percentage (0.0f - 100.0f)
    
    // Battery drain/charge rates (percentage per second)
    const float batteryDrainRate = 8.0f;   // Reduced from 20% to 8% per second when power is ON
    const float batteryChargeRate = 15.0f; // 15% per second when power is OFF
};


