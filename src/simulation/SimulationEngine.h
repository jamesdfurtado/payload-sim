#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <raylib.h>

struct SimulationState {
    // Core simulation flags
    bool canLaunchAuthorized = false;
    bool targetValidated = false;
    bool targetAcquired = false;
    bool depthClearanceMet = false;
    bool launchTubeIntegrity = true;
    bool payloadSystemOperational = true;
    bool powerSupplyStable = true;
    bool noFriendlyUnitsInBlastRadius = true;
    bool launchConditionsFavorable = true;

    // Dynamic values
    float submarineDepthMeters = 100.0f;
    float optimalLaunchDepthMeters = 120.0f;
    float batteryPercent = 100.0f;
    Vector2 submarinePosition = { 0.0f, 0.0f };
};

class ISimSystem {
public:
    virtual ~ISimSystem() = default;
    virtual const char* getName() const = 0;
    virtual void update(SimulationState& state, float dt) = 0;
};

class SimulationEngine {
public:
    SimulationEngine();

    void registerSystem(std::shared_ptr<ISimSystem> system);
    void update(float dt);

    SimulationState& getState();
    const SimulationState& getState() const;

private:
    SimulationState state;
    std::vector<std::shared_ptr<ISimSystem>> systems;
};


