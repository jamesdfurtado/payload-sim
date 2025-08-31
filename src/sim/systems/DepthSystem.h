#pragma once

#include "../ISystem.h"
#include <algorithm>
#include <random>

class DepthSystem : public ISystem {
public:
    DepthSystem() {
        // generate random optimal depth
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(50.0f, 200.0f);
        optimalDepth = dis(gen);
        
        // generate random spawn depth (+/-30m of optimal depth)
        std::uniform_real_distribution<float> spawnDis(-30.0f, 30.0f);
        float spawnOffset = spawnDis(gen);
        desiredDepth = optimalDepth + spawnOffset;
        
        // Ensure spawn depth is within valid bounds
        desiredDepth = std::clamp(desiredDepth, minDepth, maxDepth);
    }
    
    const char* getName() const override { return "DepthSystem"; }
    
    void update(SimulationState& state, float dt) override {
        // depth throttle handling
        if (depthChange != 0.0f) {
            desiredDepth += depthChange * depthChangeRate * dt;
            desiredDepth = std::clamp(desiredDepth, minDepth, maxDepth);
        }
        
        // Update simulation state
        state.currentDepthMeters = desiredDepth;

        // has user reached desired depth?
        state.depthClearanceMet = state.currentDepthMeters >= (optimalDepth - 5.0f) && state.currentDepthMeters <= (optimalDepth + 5.0f);
    }

    void setDepth(float meters) { desiredDepth = std::clamp(meters, minDepth, maxDepth); }
    float getDepth() const { return desiredDepth; }
    
    void setDepthChange(float change) { depthChange = std::clamp(change, -1.0f, 1.0f); }
    void setThrottleValue(float throttleValue) { 
        float depthChange = (throttleValue - 0.5f) * 1.0f;
        setDepthChange(depthChange);
    }
    float getOptimalDepth() const { return optimalDepth; }
    
    const char* getMovementStatus() const {
        if (depthChange > 0.05f) return "ASCENDING";
        else if (depthChange < -0.05f) return "DESCENDING";
        else return "HOLDING";
    }
    
    float getThrottlePercentage() const {
        return (depthChange + 0.5f) * 100.0f;
    }

private:
    float desiredDepth;
    float depthChange = 0.0f;
    
    const float depthChangeRate = 16.67f;
    const float minDepth = 10.0f;
    const float maxDepth = 500.0f;
    float optimalDepth;
};


