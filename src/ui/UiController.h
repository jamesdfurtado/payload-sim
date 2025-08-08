#pragma once

#include <string>
#include <raylib.h>
#include "../simulation/SimulationEngine.h"

class UiController {
public:
    explicit UiController(SimulationEngine& engine) : engine(engine) {}

    void update(float /*dt*/) {}
    void render() {
        DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE);
    }

private:
    SimulationEngine& engine;
};


