#pragma once

#include <raylib.h>
#include <vector>
#include "../systems/ContactManager.h"

class SimulationEngine;
class SonarSystem;
class SafetySystem;

class SonarDisplay {
public:
    SonarDisplay(SimulationEngine& engine, SonarSystem* sonar, SafetySystem* safety);

    void updateMissileAnimation(float dt, MissileState& missileState, const Rectangle& sonarRect);
    // Note: Once launched, missiles maintain their trajectory and do not redirect to new targets
    void drawSonar(Rectangle r, const MissileState& missileState);
    void drawMissileTrail(const MissileState& missileState);
    void drawMouseReticle(Rectangle r);

    Vector2 worldToScreen(Vector2 worldPos, Rectangle screenRect);
    Vector2 screenToWorld(Vector2 screenPos, Rectangle screenRect);

private:
    SimulationEngine& engine;
    SonarSystem* sonar;
    SafetySystem* safety;

    void drawSubmarineIcon(Rectangle r);
    void drawSonarGrid(Rectangle r);
    void drawCrossHair(Rectangle r);
    void drawSonarContacts(Rectangle r);
    void drawTargetLock(Rectangle r);
    void drawMissile(const MissileState& missileState);
    void drawExplosion(const MissileState& missileState);
};
