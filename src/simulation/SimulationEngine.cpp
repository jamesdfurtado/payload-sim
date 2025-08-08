#include "SimulationEngine.h"

SimulationEngine::SimulationEngine() = default;

void SimulationEngine::registerSystem(std::shared_ptr<ISimSystem> system) {
    systems.push_back(std::move(system));
}

void SimulationEngine::update(float dt) {
    for (auto& sys : systems) {
        sys->update(state, dt);
    }
}

SimulationState& SimulationEngine::getState() {
    return state;
}

const SimulationState& SimulationEngine::getState() const {
    return state;
}


