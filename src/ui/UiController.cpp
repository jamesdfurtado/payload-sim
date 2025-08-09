#include "UiController.h"
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

UiController::UiController(SimulationEngine& engine,
                           SonarSystem* sonar,
                           PowerSystem* power,
                           DepthControl* depth,
                           TargetingSystem* targeting,
                           SafetySystem* safety,
                           EnvironmentSystem* environment)
    : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety), environment(environment) {
    std::srand((unsigned int)std::time(nullptr));
}

void UiController::update(float dt) {
    // Minimal stub for now
    (void)dt;
}

void UiController::render() {
    DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE);
}

void UiController::drawStatusLights(const SimulationState& s, Rectangle r) {
    (void)s; (void)r;
}

void UiController::drawPower(Rectangle r) { (void)r; }
void UiController::drawDepth(Rectangle r) { (void)r; }
void UiController::drawSonar(Rectangle r) { (void)r; }
void UiController::drawControls(Rectangle r) { (void)r; }

Vector2 UiController::worldToScreen(Vector2 p, Rectangle r) {
    float nx = (p.x + 600.0f) / 1200.0f;
    float ny = (p.y + 360.0f) / 720.0f;
    return { r.x + nx * r.width, r.y + ny * r.height };
}

Vector2 UiController::screenToWorld(Vector2 p, Rectangle r) {
    float nx = (p.x - r.x) / r.width;
    float ny = (p.y - r.y) / r.height;
    return { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
}


