#pragma once

#include <raylib.h>
#include <cmath>

class PulsatingBorder {
public:
    PulsatingBorder(Color baseColor = ORANGE, float speed = 3.0f, float minAlpha = 0.4f, float maxAlpha = 1.0f, int thickness = 2)
        : baseColor(baseColor), speed(speed), minAlpha(minAlpha), maxAlpha(maxAlpha), thickness(thickness), timer(0.0f) {}

    void update(float dt) {
        timer += dt;
    }

    void drawBorder(const Rectangle& bounds) const {
        float pulseAlpha = minAlpha + (maxAlpha - minAlpha) * (0.5f + 0.5f * sinf(timer * speed));
        DrawRectangleLinesEx(bounds, thickness, Fade(baseColor, pulseAlpha));
    }
    
    void drawFilledBorder(const Rectangle& bounds, float backgroundAlpha = 0.2f) const {
        float pulseAlpha = minAlpha + (maxAlpha - minAlpha) * (0.5f + 0.5f * sinf(timer * speed));    
        DrawRectangleRec(bounds, Fade(baseColor, backgroundAlpha));
        DrawRectangleLinesEx(bounds, thickness, Fade(baseColor, pulseAlpha));
    }

    void setColor(Color color) { baseColor = color; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setAlphaRange(float min, float max) { minAlpha = min; maxAlpha = max; }
    void setThickness(int newThickness) { thickness = newThickness; }

private:
    Color baseColor;
    float speed;
    float minAlpha;
    float maxAlpha;
    int thickness;
    float timer;
};
