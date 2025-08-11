#pragma once

#include <raylib.h>
#include "ContactManager.h"

class MissileRenderer {
public:
    MissileRenderer();
    
    // Main rendering methods
    void renderMissile(const MissileState& missileState, const Rectangle& screenRect);
    void renderMissileTrail(const MissileState& missileState, const Rectangle& screenRect);
    void renderExplosion(const MissileState& missileState, const Rectangle& screenRect);
    
    // Coordinate conversion utilities
    Vector2 worldToScreen(Vector2 worldPos, const Rectangle& screenRect) const;
    Vector2 screenToWorld(Vector2 screenPos, const Rectangle& screenRect) const;
    
    // Get screen-converted missile state for UI
    MissileState getScreenMissileState(const MissileState& worldMissileState, const Rectangle& screenRect) const;

private:
    // Rendering constants
    static constexpr float EXPLOSION_DURATION = 0.4f;
    static constexpr int MISSILE_RADIUS = 6;
    static constexpr int EXPLOSION_MAX_RADIUS = 24;
    static constexpr int EXPLOSION_MIN_RADIUS = 12;
    
    // Colors
    static constexpr Color MISSILE_COLOR = ORANGE;
    static constexpr Color TRAIL_COLOR = GRAY;
    static constexpr Color EXPLOSION_OUTER_COLOR = YELLOW;
    static constexpr Color EXPLOSION_INNER_COLOR = RED;
};
