#pragma once

#include "../ISystem.h"
#include <algorithm>
#include <random>

class DepthSystem : public ISystem {
public:
    DepthSystem() {
        // Generate random optimal depth between 50 and 200 meters
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(50.0f, 200.0f);
        optimalDepth = dis(gen);
        
        // Generate random spawn depth within 30m above or below optimal depth
        std::uniform_real_distribution<float> spawnDis(-30.0f, 30.0f);
        float spawnOffset = spawnDis(gen);
        desiredDepth = optimalDepth + spawnOffset;
        
        // Ensure spawn depth is within valid bounds
        desiredDepth = std::clamp(desiredDepth, minDepth, maxDepth);
    }
    
    const char* getName() const override { return "DepthSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // Apply depth change based on throttle
        if (depthChange != 0.0f) {
            desiredDepth += depthChange * depthChangeRate * dt;
            desiredDepth = std::clamp(desiredDepth, minDepth, maxDepth);
        }
        
        // Update simulation state
        state.currentDepthMeters = desiredDepth;
        // Depth clearance requires being within 5 meters of optimal depth (110.2m ± 5m = 105.2m to 115.2m)
        state.depthClearanceMet = state.currentDepthMeters >= (optimalDepth - 5.0f) && state.currentDepthMeters <= (optimalDepth + 5.0f);
    }

    void setDepth(float meters) { desiredDepth = std::clamp(meters, minDepth, maxDepth); }
    float getDepth() const { return desiredDepth; }
    
    void setDepthChange(float change) { depthChange = std::clamp(change, -1.0f, 1.0f); }
    float getOptimalDepth() const { return optimalDepth; }

private:
    float desiredDepth;         // Initial spawn depth (randomly set relative to optimal depth)
    float depthChange = 0.0f;  // -1.0f = DOWN, 0.0f = NEUTRAL, 1.0f = UP
    
    const float depthChangeRate = 16.67f;  // meters per second (reduced from 50.0f for 3x less sensitivity)
    const float minDepth = 10.0f;         // minimum depth in meters
    const float maxDepth = 500.0f;        // maximum depth in meters
    float optimalDepth;                   // optimal launch depth in meters (randomly generated between 50-200m)
};


