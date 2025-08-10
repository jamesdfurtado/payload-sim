#pragma once

#include <raylib.h>

class SimulationEngine;
class PowerSystem;

class StatusPanel {
public:
    StatusPanel(SimulationEngine& engine, PowerSystem* power);

    void drawStatusLights(Rectangle r);
    void drawPower(Rectangle r, float uiWeaponsPower);
    void drawDepth(Rectangle r);

private:
    SimulationEngine& engine;
    PowerSystem* power;
};
