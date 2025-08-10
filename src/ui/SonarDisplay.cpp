#include "SonarDisplay.h"
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/SafetySystem.h"
#include <cmath>

SonarDisplay::SonarDisplay(SimulationEngine& engine, SonarSystem* sonar, SafetySystem* safety)
    : engine(engine), sonar(sonar), safety(safety) {
}

void SonarDisplay::updateMissileAnimation(float dt, MissileState& missileState, const Rectangle& sonarRect) {
    if (!missileState.active) {
        // Check for new missile launch
        static bool prevLaunched = false;
        bool nowLaunched = (safety && safety->getPhase() == LaunchPhase::Launched);
        if (nowLaunched && !prevLaunched && sonar && sonar->isTargetAcquired()) {
            missileState.active = true;
            missileState.trail.clear();
            missileState.position = worldToScreen({0,0}, sonarRect); // launch from sub center
            missileState.target = worldToScreen(sonar->getLockedTarget(), sonarRect);
            missileState.progress = 0.0f;
            missileState.explosionTimer = 0.0f;
            
            // Find target index for heat-seeking
            const auto& contacts = sonar->getContacts();
            missileState.targetIndex = -1;
            for (int i = 0; i < (int)contacts.size(); ++i) {
                if (contacts[i].type == ContactType::EnemySub && 
                    fabsf(contacts[i].position.x - sonar->getLockedTarget().x) < 1e-2f && 
                    fabsf(contacts[i].position.y - sonar->getLockedTarget().y) < 1e-2f) {
                    missileState.targetIndex = i;
                    break;
                }
            }
        }
        prevLaunched = nowLaunched;
        return;
    }

    // Handle explosion phase
    if (missileState.explosionTimer > 0.0f) {
        missileState.explosionTimer -= dt;
        if (missileState.explosionTimer <= 0.0f && sonar && missileState.targetIndex >= 0) {
            // Remove enemy and disengage lock
            sonar->removeContact(missileState.targetIndex);
            missileState.active = false;
        }
        return;
    }

    // Handle missile flight
    Vector2 targetWorld = {0,0};
    if (sonar && missileState.targetIndex >= 0) {
        const auto& contacts = sonar->getContacts();
        if (missileState.targetIndex < (int)contacts.size() && contacts[missileState.targetIndex].type == ContactType::EnemySub) {
            targetWorld = contacts[missileState.targetIndex].position;
        } else {
            // Target gone, just finish at last known position
            targetWorld = screenToWorld(missileState.target, sonarRect);
        }
    }
    
    missileState.target = worldToScreen(targetWorld, sonarRect);
    float speed = MISSILE_SPEED;
    Vector2 toTarget = {missileState.target.x - missileState.position.x, missileState.target.y - missileState.position.y};
    float dist = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    
    if (dist > 1.0f) {
        missileState.velocity = {toTarget.x / dist * speed * dt, toTarget.y / dist * speed * dt};
        missileState.position.x += missileState.velocity.x;
        missileState.position.y += missileState.velocity.y;
        missileState.trail.push_back(missileState.position);
        if (missileState.trail.size() > 40) missileState.trail.erase(missileState.trail.begin());
        
        if (dist < 16.0f) {
            missileState.position = missileState.target;
            missileState.explosionTimer = EXPLOSION_DURATION;
        }
    } else {
        missileState.position = missileState.target;
        missileState.explosionTimer = EXPLOSION_DURATION;
    }
}

void SonarDisplay::drawSonar(Rectangle r, const MissileState& missileState) {
    DrawRectangleRec(r, Fade(BLUE, 0.2f));
    DrawRectangleLinesEx(r, 1, SKYBLUE);
    DrawText("Sonar", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);

    drawSubmarineIcon(r);
    drawSonarContacts(r);
    drawTargetLock(r);
    
    if (missileState.active) {
        drawMissileTrail(missileState);
        drawMissile(missileState);
        if (missileState.explosionTimer > 0.0f) {
            drawExplosion(missileState);
        }
    }
    
    drawMouseReticle(r);
}

void SonarDisplay::drawSubmarineIcon(Rectangle r) {
    Vector2 subCenter = worldToScreen({0,0}, r);
    DrawRectangle((int)subCenter.x - 10, (int)subCenter.y - 6, 20, 12, DARKGRAY);
    DrawTriangle(
        Vector2{subCenter.x + 10.0f, subCenter.y},
        Vector2{subCenter.x + 18.0f, subCenter.y - 8.0f},
        Vector2{subCenter.x + 18.0f, subCenter.y + 8.0f},
        GRAY);
    DrawCircle((int)subCenter.x - 8, (int)subCenter.y, 4, SKYBLUE);
}

void SonarDisplay::drawSonarContacts(Rectangle r) {
    if (!sonar) return;
    
    for (auto& c : sonar->getContacts()) {
        Color color = LIGHTGRAY;
        switch (c.type) {
            case ContactType::EnemySub: color = RED; break;
            case ContactType::FriendlySub: color = GREEN; break;
            case ContactType::Fish: color = BLUE; break;
            case ContactType::Debris: color = GRAY; break;
        }
        Vector2 sp = worldToScreen(c.position, r);
        DrawCircle((int)sp.x, (int)sp.y, 4, color);
    }
}

void SonarDisplay::drawTargetLock(Rectangle r) {
    if (engine.getState().targetAcquired && sonar) {
        Vector2 sp = worldToScreen(sonar->getLockedTarget(), r);
        DrawCircleLines((int)sp.x, (int)sp.y, 12, YELLOW);
        DrawLine((int)sp.x - 16, (int)sp.y, (int)sp.x + 16, (int)sp.y, YELLOW);
        DrawLine((int)sp.x, (int)sp.y - 16, (int)sp.x, (int)sp.y + 16, YELLOW);
    }
}

void SonarDisplay::drawMissileTrail(const MissileState& missileState) {
    for (size_t i = 1; i < missileState.trail.size(); ++i) {
        DrawLine((int)missileState.trail[i-1].x, (int)missileState.trail[i-1].y, 
                 (int)missileState.trail[i].x, (int)missileState.trail[i].y, 
                 Fade(GRAY, 0.5f));
    }
}

void SonarDisplay::drawMissile(const MissileState& missileState) {
    DrawCircle((int)missileState.position.x, (int)missileState.position.y, 6, ORANGE);
}

void SonarDisplay::drawExplosion(const MissileState& missileState) {
    float t = 1.0f - missileState.explosionTimer / EXPLOSION_DURATION;
    DrawCircle((int)missileState.position.x, (int)missileState.position.y, 24 * t, YELLOW);
    DrawCircle((int)missileState.position.x, (int)missileState.position.y, 12 * t, RED);
}

void SonarDisplay::drawMouseReticle(Rectangle r) {
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, r)) {
        DrawCircleLines((int)m.x, (int)m.y, 10, Fade(YELLOW, 0.5f));
    }
}

Vector2 SonarDisplay::worldToScreen(Vector2 worldPos, Rectangle screenRect) {
    float nx = (worldPos.x + 600.0f) / 1200.0f;
    float ny = (worldPos.y + 360.0f) / 720.0f;
    return { screenRect.x + nx * screenRect.width, screenRect.y + ny * screenRect.height };
}

Vector2 SonarDisplay::screenToWorld(Vector2 screenPos, Rectangle screenRect) {
    float nx = (screenPos.x - screenRect.x) / screenRect.width;
    float ny = (screenPos.y - screenRect.y) / screenRect.height;
    return { nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
}
