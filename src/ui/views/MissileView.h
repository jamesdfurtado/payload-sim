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
            float scale = sonarBounds.width / 1200.0f;
            
            int x = (int)screen.x;
            int y = (int)screen.y;
            
            // Calculate ring sizes in screen coordinates
            int innerRadius = (int)(explosion.innerRing * scale);
            int middleRadius = (int)(explosion.middleRing * scale);
            int outerRadius = (int)(explosion.outerRing * scale);
            
            // Calculate color intensities based on time
            float progress = explosion.timer / explosion.duration;
            float pulseIntensity = 0.5f + 0.5f * sinf(progress * 10.0f); // Pulsing effect
            
            // Initial bright flash (only at the very beginning)
            if (explosion.flashIntensity > 0.8f) {
                DrawCircle(x, y, (int)(20 * scale), Fade(WHITE, explosion.flashIntensity));
            }
            
            // Outer ring - red, appears last
            if (outerRadius > 5) {
                Color outerColor = {255, 100, 50, (unsigned char)(255 * (1.0f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, outerRadius, outerColor);
                DrawCircleLines(x, y, outerRadius - 1, Fade(outerColor, 0.5f));
            }
            
            // Middle ring - orange, appears second
            if (middleRadius > 3) {
                Color middleColor = {255, 150, 50, (unsigned char)(255 * (1.2f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, middleRadius, middleColor);
                DrawCircleLines(x, y, middleRadius - 1, Fade(middleColor, 0.7f));
                DrawCircleLines(x, y, middleRadius + 1, Fade(middleColor, 0.3f));
            }
            
            // Inner ring - bright yellow/white, appears first
            if (innerRadius > 1) {
                Color innerColor = {255, 255, 150, (unsigned char)(255 * (1.5f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, innerRadius, innerColor);
                DrawCircleLines(x, y, innerRadius - 1, Fade(innerColor, 0.8f));
                DrawCircleLines(x, y, innerRadius + 1, Fade(innerColor, 0.5f));
                DrawCircleLines(x, y, innerRadius + 2, Fade(innerColor, 0.2f));
            }
            
            // Central core - always bright while explosion is active
            Color coreColor = {255, 255, 200, (unsigned char)(255 * (1.0f - progress * 0.7f) * pulseIntensity)};
            DrawCircle(x, y, (int)(8 * scale), Fade(coreColor, 0.6f));
            DrawCircle(x, y, (int)(4 * scale), coreColor);
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
