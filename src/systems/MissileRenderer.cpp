#include "MissileRenderer.h"
#include <cmath>

MissileRenderer::MissileRenderer() {
}

void MissileRenderer::renderMissile(const MissileState& missileState, const Rectangle& screenRect) {
    if (!missileState.active) {
        return;
    }
    
    // Convert world coordinates to screen coordinates
    Vector2 screenPos = worldToScreen(missileState.position, screenRect);
    
    // Draw the missile
    DrawCircle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), MISSILE_RADIUS, MISSILE_COLOR);
    
    // Draw trail with screen coordinates
    renderMissileTrail(missileState, screenRect);
    
    // Draw explosion if active (with screen coordinates)
    if (missileState.explosionTimer > 0.0f) {
        renderExplosion(missileState, screenRect);
    }
}

void MissileRenderer::renderMissileTrail(const MissileState& missileState, const Rectangle& screenRect) {
    if (missileState.trail.size() < 2) {
        return;
    }
    
    // Convert trail positions to screen coordinates
    std::vector<Vector2> screenTrail;
    for (const auto& worldPos : missileState.trail) {
        screenTrail.push_back(worldToScreen(worldPos, screenRect));
    }
    
    // Draw trail lines between positions
    for (size_t i = 1; i < screenTrail.size(); ++i) {
        DrawLineEx(screenTrail[i-1], screenTrail[i], 2.0f, Fade(TRAIL_COLOR, 0.5f));
    }
    
    // If there's only one position in trail, draw a small dot to show the trail start
    if (screenTrail.size() == 1) {
        DrawCircle(static_cast<int>(screenTrail[0].x), static_cast<int>(screenTrail[0].y), 2, Fade(TRAIL_COLOR, 0.3f));
    }
}

void MissileRenderer::renderExplosion(const MissileState& missileState, const Rectangle& screenRect) {
    if (missileState.explosionTimer <= 0.0f) {
        return;
    }
    
    // Convert position to screen coordinates for explosion
    Vector2 screenPos = worldToScreen(missileState.position, screenRect);
    
    float t = 1.0f - missileState.explosionTimer / EXPLOSION_DURATION;
    int outerRadius = static_cast<int>(EXPLOSION_MAX_RADIUS * t);
    int innerRadius = static_cast<int>(EXPLOSION_MIN_RADIUS * t);
    
    // Draw explosion circles
    DrawCircle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), outerRadius, EXPLOSION_OUTER_COLOR);
    DrawCircle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), innerRadius, EXPLOSION_INNER_COLOR);
}

Vector2 MissileRenderer::worldToScreen(Vector2 worldPos, const Rectangle& screenRect) const {
    float nx = (worldPos.x + 600.0f) / 1200.0f;
    float ny = (worldPos.y + 360.0f) / 720.0f;
    return { screenRect.x + nx * screenRect.width, screenRect.y + ny * screenRect.height };
}

Vector2 MissileRenderer::screenToWorld(Vector2 screenPos, const Rectangle& screenRect) const {
    float nx = (screenPos.x - screenRect.x) / screenRect.width;
    float ny = (screenPos.y - screenRect.y) / screenRect.height;
    return { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
}

MissileState MissileRenderer::getScreenMissileState(const MissileState& worldMissileState, const Rectangle& screenRect) const {
    MissileState screenState = worldMissileState;
    
    if (worldMissileState.active) {
        // Convert position and target to screen coordinates
        screenState.position = worldToScreen(worldMissileState.position, screenRect);
        screenState.target = worldToScreen(worldMissileState.target, screenRect);
        
        // Convert trail positions to screen coordinates
        screenState.trail.clear();
        for (const auto& worldPos : worldMissileState.trail) {
            screenState.trail.push_back(worldToScreen(worldPos, screenRect));
        }
    }
    
    return screenState;
}
