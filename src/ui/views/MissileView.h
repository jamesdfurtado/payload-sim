#pragma once

#include "../Widget.h"
#include "../../sim/world/MissileManager.h"
#include <iostream>

class MissileView : public Widget {
public:
    explicit MissileView(MissileManager& missiles) : missileManager(missiles) {}

//    void draw() const override {
//    }

    // draw missiles and explosions on sonar
    void drawMissilesOnSonar(const Rectangle& sonarBounds) const {
        int missileCount = static_cast<int>(missileManager.getActiveMissiles().size());
        
        // draw active missiles
        for (const auto& missile : missileManager.getActiveMissiles()) {
            Vector2 screen = worldToScreen(missile.position, sonarBounds);
            DrawCircle((int)screen.x, (int)screen.y, 3, YELLOW);
            
            // draw trail showing path
            if (missile.trailPoints.size() > 1) {
                for (size_t i = 1; i < missile.trailPoints.size(); ++i) {
                    Vector2 start = worldToScreen(missile.trailPoints[i-1], sonarBounds);
                    Vector2 end = worldToScreen(missile.trailPoints[i], sonarBounds);
                    
                    float fadeRatio = (float)i / (float)missile.trailPoints.size();
                    float alpha = 0.3f + (fadeRatio * 0.4f);

                    DrawLineEx(start, end, 2, Fade(GRAY, alpha));
                }
            }
        }
        
        // draw explosion
        for (const auto& explosion : missileManager.getActiveExplosions()) {
            Vector2 screen = worldToScreen(explosion.position, sonarBounds);
            float scale = sonarBounds.width / 1200.0f;
            
            int x = (int)screen.x;
            int y = (int)screen.y;

            int innerRadius = (int)(explosion.innerRing * scale);
            int middleRadius = (int)(explosion.middleRing * scale);
            int outerRadius = (int)(explosion.outerRing * scale);
            

            float progress = explosion.timer / explosion.duration;
            float pulseIntensity = 0.5f + 0.5f * sinf(progress * 10.0f);
            
            if (explosion.flashIntensity > 0.8f) {
                DrawCircle(x, y, (int)(20 * scale), Fade(WHITE, explosion.flashIntensity));
            }
            
            if (outerRadius > 5) {
                Color outerColor = {255, 100, 50, (unsigned char)(255 * (1.0f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, outerRadius, outerColor);
                DrawCircleLines(x, y, outerRadius - 1, Fade(outerColor, 0.5f));
            }
            
            if (middleRadius > 3) {
                Color middleColor = {255, 150, 50, (unsigned char)(255 * (1.2f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, middleRadius, middleColor);
                DrawCircleLines(x, y, middleRadius - 1, Fade(middleColor, 0.7f));
                DrawCircleLines(x, y, middleRadius + 1, Fade(middleColor, 0.3f));
            }
            
            if (innerRadius > 1) {
                Color innerColor = {255, 255, 150, (unsigned char)(255 * (1.5f - progress) * pulseIntensity)};
                DrawCircleLines(x, y, innerRadius, innerColor);
                DrawCircleLines(x, y, innerRadius - 1, Fade(innerColor, 0.8f));
                DrawCircleLines(x, y, innerRadius + 1, Fade(innerColor, 0.5f));
                DrawCircleLines(x, y, innerRadius + 2, Fade(innerColor, 0.2f));
            }
            
            Color coreColor = {255, 255, 200, (unsigned char)(255 * (1.0f - progress * 0.7f) * pulseIntensity)};
            DrawCircle(x, y, (int)(8 * scale), Fade(coreColor, 0.6f));
            DrawCircle(x, y, (int)(4 * scale), coreColor);
        }
    }

private:
    // world to screen conversion
    static Vector2 worldToScreen(Vector2 world, const Rectangle& r) {
        float nx = (world.x + 600.0f) / 1200.0f;
        float ny = (world.y + 360.0f) / 720.0f;
        return { r.x + nx * r.width, r.y + ny * r.height };
    }

    MissileManager& missileManager;
};
