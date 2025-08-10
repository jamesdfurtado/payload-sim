#include "SimulationEngine.h"
#include <cstdlib>
#include <ctime>

SimulationEngine::SimulationEngine() {
    // Initialize random seed
    std::srand((unsigned int)std::time(nullptr));
    
    // Randomize initial depth scenario
    randomizeDepthScenario();
}

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

void SimulationEngine::randomizeDepthScenario() {
    // Generate random starting depth between 60m and 180m
    float startingDepth = 60.0f + (std::rand() % 121); // 60-180m range
    
    // Generate target depth within 30m of starting depth
    float minTarget = startingDepth - 30.0f;
    float maxTarget = startingDepth + 30.0f;
    
    // Ensure target stays within reasonable submarine operating limits (20-300m)
    if (minTarget < 20.0f) minTarget = 20.0f;
    if (maxTarget > 300.0f) maxTarget = 300.0f;
    
    // Generate random target within the valid range
    float targetRange = maxTarget - minTarget;
    float targetDepth = minTarget + (targetRange * (std::rand() % 100) / 100.0f);
    
    // Apply the randomized values
    state.submarineDepthMeters = startingDepth;
    state.optimalLaunchDepthMeters = targetDepth;
}


