#pragma once

#include <raylib.h>
#include <vector>

class SimulationEngine;
class SonarSystem;
class SafetySystem;

class SonarDisplay {
public:
    SonarDisplay(SimulationEngine& engine, SonarSystem* sonar, SafetySystem* safety);

    struct MissileState {
        bool active = false;
        Vector2 position{0,0};
        Vector2 velocity{0,0};
        Vector2 target{0,0};
        std::vector<Vector2> trail;
        float progress = 0.0f;
        float explosionTimer = 0.0f;
        int targetIndex = -1;
    };

    void updateMissileAnimation(float dt, MissileState& missileState, const Rectangle& sonarRect);
    void drawSonar(Rectangle r, const MissileState& missileState);
    void drawMissileTrail(const MissileState& missileState);
    void drawMouseReticle(Rectangle r);

    Vector2 worldToScreen(Vector2 worldPos, Rectangle screenRect);
    Vector2 screenToWorld(Vector2 screenPos, Rectangle screenRect);

private:
    SimulationEngine& engine;
    SonarSystem* sonar;
    SafetySystem* safety;

    static constexpr float MISSILE_SPEED = 200.0f;
    static constexpr float EXPLOSION_DURATION = 0.6f;

    void drawSubmarineIcon(Rectangle r);
    void drawSonarGrid(Rectangle r);
    void drawCrossHair(Rectangle r);
    void drawSonarContacts(Rectangle r);
    void drawTargetLock(Rectangle r);
    void drawMissile(const MissileState& missileState);
    void drawExplosion(const MissileState& missileState);
};
