#pragma once

#include <vector>
#include <memory>
#include "SimulationState.h"
#include "ISystem.h"

class SimulationEngine {
public:
    SimulationEngine() = default;

    void update(float dt) {
        for (const auto& system : systems) {
            system->update(state, dt);
        }
    }

    void registerSystem(const std::shared_ptr<ISystem>& system) {
        systems.push_back(system);
    }

    SimulationState& getState() { return state; }
    const SimulationState& getState() const { return state; }

private:
    SimulationState state{};
    std::vector<std::shared_ptr<ISystem>> systems;
};