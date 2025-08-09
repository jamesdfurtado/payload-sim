#pragma once

#include <string>
#include <raylib.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/PowerSystem.h"
#include "../systems/DepthControl.h"
#include "../systems/TargetingSystem.h"
#include "../systems/SafetySystem.h"

class UiController {
public:
    UiController(SimulationEngine& engine,
                 SonarSystem* sonar,
                 PowerSystem* power,
                 DepthControl* depth,
                 TargetingSystem* targeting,
                 SafetySystem* safety,
                 EnvironmentSystem* environment)
        : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety), environment(environment) {}

    void update(float dt) {}
    void render() { DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE); }

private:
    SimulationEngine& engine;
    SonarSystem* sonar = nullptr;
    PowerSystem* power = nullptr;
    DepthControl* depth = nullptr;
    TargetingSystem* targeting = nullptr;
    SafetySystem* safety = nullptr;
    EnvironmentSystem* environment = nullptr;
};


