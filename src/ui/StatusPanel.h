#pragma once

#include <raylib.h>

class SimulationEngine;
class PowerSystem;

class StatusPanel {
public:
    StatusPanel(SimulationEngine& engine, PowerSystem* power);

    void drawStatusLights(Rectangle r);
    void drawPower(Rectangle r, float& uiPowerLevel); // Changed to reference for interactive control
    void drawDepth(Rectangle r);

private:
    SimulationEngine& engine;
    PowerSystem* power;
};
