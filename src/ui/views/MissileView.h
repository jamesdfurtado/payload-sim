#pragma once

#include "../Widget.h"
#include "../../sim/world/MissileManager.h"
#include <iostream>

class MissileView : public Widget {
public:
    explicit MissileView(MissileManager& missiles) : missileManager(missiles) {}

    void draw() const override {
        // This view doesn't draw its own panel - it only renders missiles on sonar
    }

    // This method renders missiles and explosions on the sonar display
    void drawMissilesOnSonar(const Rectangle& sonarBounds) const {
        // Debug: Log missile count
        int missileCount = static_cast<int>(missileManager.getActiveMissiles().size());
        
        // Draw missiles
        for (const auto& missile : missileManager.getActiveMissiles()) {
            Vector2 screen = worldToScreen(missile.position, sonarBounds);
            DrawCircle((int)screen.x, (int)screen.y, 3, YELLOW);
            
            // Draw missile trail
            Vector2 trailEnd = { missile.position.x - missile.velocity.x * 0.1f, 
                                missile.position.y - missile.velocity.y * 0.1f };
            Vector2 trailScreen = worldToScreen(trailEnd, sonarBounds);
            DrawLineEx(screen, trailScreen, 2, Fade(YELLOW, 0.6f));
        }
        
        // Draw explosions
        for (const auto& explosion : missileManager.getActiveExplosions()) {
            Vector2 screen = worldToScreen(explosion.position, sonarBounds);
            float screenRadius = explosion.radius * (sonarBounds.width / 1200.0f);
            
            // Draw expanding explosion circle
            DrawCircleLines((int)screen.x, (int)screen.y, (int)screenRadius, ORANGE);
            DrawCircle((int)screen.x, (int)screen.y, (int)(screenRadius * 0.7f), Fade(ORANGE, 0.3f));
            
            // Draw explosion particles
            for (int i = 0; i < 8; ++i) {
                float angle = (float)i * PI / 4.0f;
                float particleDist = screenRadius * 0.8f;
                Vector2 particlePos = { screen.x + cosf(angle) * particleDist, 
                                      screen.y + sinf(angle) * particleDist };
                DrawCircle((int)particlePos.x, (int)particlePos.y, 2, RED);
            }
        }
    }

private:
    // Coordinate transform: world (-600..600, -360..360) to screen inside rectangle
    static Vector2 worldToScreen(Vector2 world, const Rectangle& r) {
        float nx = (world.x + 600.0f) / 1200.0f;
        float ny = (world.y + 360.0f) / 720.0f;
        return { r.x + nx * r.width, r.y + ny * r.height };
    }

    MissileManager& missileManager;
};
