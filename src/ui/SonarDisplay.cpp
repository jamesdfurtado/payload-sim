#include "SonarDisplay.h"
#include "../simulation/SimulationEngine.h"
#include "../systems/SonarSystem.h"
#include "../systems/SafetySystem.h"
#include "../systems/ContactManager.h"
#include <cmath>

// Helper function to calculate distance between two Vector2 points
static float calculateDistance(const Vector2& a, const Vector2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

SonarDisplay::SonarDisplay(SimulationEngine& engine, SonarSystem* sonar, SafetySystem* safety, ContactManager* contactManager)
    : engine(engine), sonar(sonar), safety(safety), contactManager(contactManager) {
}

void SonarDisplay::updateMissileAnimation(float dt, MissileState& missileState, const Rectangle& sonarRect) {
    if (!missileState.active) {
        // Check for new missile launch
        bool nowLaunched = (safety && safety->getPhase() == LaunchPhase::Launched);
        bool hasTarget = (sonar && sonar->isTargetAcquired());
        
        // Launch missile if we're in launched phase and have a target
        if (nowLaunched && hasTarget && contactManager) {
            uint32_t targetId = sonar->getLockedTargetId();
            if (targetId != 0) {
                contactManager->launchMissileAtTarget(targetId, {0, 0}); // Launch from submarine center
            }
        }
    }

    // ALWAYS get the latest missile state from ContactManager every frame
    // This ensures the UI stays in sync with the actual missile simulation
    if (contactManager) {
        const auto& contactManagerState = contactManager->getMissileState();
        
        // Update the UI missile state with the latest from ContactManager
        missileState = contactManagerState;
        
        // Handle special cases for UI consistency
        if (missileState.explosionTimer <= 0.0f && missileState.active) {
            // Ensure UI reflects the completed explosion state
            missileState.active = false;
            missileState.trail.clear();
        }
    }
}

void SonarDisplay::drawSonar(Rectangle r, const MissileState& missileState) {
    DrawRectangleRec(r, Fade(BLUE, 0.2f));
    DrawRectangleLinesEx(r, 1, SKYBLUE);
    DrawText("Sonar", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);

    drawSonarGrid(r);
    drawCrossHair(r);
    drawSubmarineIcon(r);
    drawSonarContacts(r);
    drawTargetLock(r);
    
    // Draw missile if active
    drawMissile(r, missileState);
    
    drawMouseReticle(r);
}

void SonarDisplay::drawMissile(Rectangle r, const MissileState& missileState) {
    if (!missileState.active) {
        return;
    }
    
    // Draw missile trail (fade from bright to dim)
    if (!missileState.trail.empty()) {
        for (size_t i = 0; i < missileState.trail.size(); ++i) {
            float alpha = 0.3f + (0.7f * i / missileState.trail.size()); // Fade from dim to bright
            Color trailColor = Fade(ORANGE, alpha);
            
            Vector2 screenPos = worldToScreen(missileState.trail[i], r);
            DrawCircle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 2, trailColor);
        }
    }
    
    // Draw missile body
    Vector2 missileScreenPos = worldToScreen(missileState.position, r);
    Color missileColor = (missileState.explosionTimer > 0.0f) ? RED : ORANGE;
    
    if (missileState.explosionTimer > 0.0f) {
        // Draw explosion effect
        float explosionRadius = 20.0f * (1.0f - missileState.explosionTimer / 0.4f); // 0.4f is EXPLOSION_DURATION
        DrawCircle(static_cast<int>(missileScreenPos.x), static_cast<int>(missileScreenPos.y), 
                  static_cast<int>(explosionRadius), Fade(RED, 0.8f));
        DrawCircle(static_cast<int>(missileScreenPos.x), static_cast<int>(missileScreenPos.y), 
                  static_cast<int>(explosionRadius * 0.6f), Fade(YELLOW, 0.9f));
    } else {
        // Draw active missile
        DrawCircle(static_cast<int>(missileScreenPos.x), static_cast<int>(missileScreenPos.y), 4, missileColor);
        DrawCircleLines(static_cast<int>(missileScreenPos.x), static_cast<int>(missileScreenPos.y), 6, WHITE);
    }
}

void SonarDisplay::drawSubmarineIcon(Rectangle r) {
    Vector2 subCenter = worldToScreen({0,0}, r);
    
    // Draw main submarine body (rounded rectangle effect using multiple circles)
    int bodyWidth = 24;
    int bodyHeight = 14;
    int bodyX = static_cast<int>(subCenter.x) - static_cast<int>(bodyWidth/2);
    int bodyY = static_cast<int>(subCenter.y) - static_cast<int>(bodyHeight/2);
    
    // Main body rectangle
    DrawRectangle(bodyX + 2, bodyY, bodyWidth - 4, bodyHeight, DARKGRAY);
    
    // Rounded ends using circles
    DrawCircle(bodyX + 2, static_cast<int>(subCenter.y), static_cast<int>(bodyHeight/2), DARKGRAY);
    DrawCircle(bodyX + bodyWidth - 2, static_cast<int>(subCenter.y), static_cast<int>(bodyHeight/2), DARKGRAY);
    
    // Draw submarine tower (conning tower) on top, positioned towards the left side
    int towerWidth = 6; // Made skinnier (was 8)
    int towerHeight = 6; // Made shorter (was 8)
    int towerX = bodyX + 4; // Position more towards the left side (was bodyX + static_cast<int>(bodyWidth/2) - 2)
    int towerY = bodyY - towerHeight;
    
    // Tower base
    DrawRectangle(towerX, towerY + 2, towerWidth, towerHeight - 2, GRAY);
    
    // Rounded top of tower
    DrawCircle(static_cast<int>(towerX + towerWidth/2), towerY + 2, static_cast<int>(towerWidth/2), GRAY);
    
    // Add a small periscope detail on top of the tower
    DrawCircle(static_cast<int>(towerX + towerWidth/2), towerY, 2, DARKGRAY);
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
        DrawCircle(static_cast<int>(sp.x), static_cast<int>(sp.y), 4, color);
    }
}

void SonarDisplay::drawTargetLock(Rectangle r) {
    if (engine.getState().targetAcquired && sonar) {
        Vector2 sp = worldToScreen(sonar->getLockedTarget(), r);
        DrawCircleLines(static_cast<int>(sp.x), static_cast<int>(sp.y), 12, YELLOW);
        DrawLine(static_cast<int>(sp.x) - 16, static_cast<int>(sp.y), static_cast<int>(sp.x) + 16, static_cast<int>(sp.y), YELLOW);
        DrawLine(static_cast<int>(sp.x), static_cast<int>(sp.y) - 16, static_cast<int>(sp.x), static_cast<int>(sp.y) + 16, YELLOW);
    }
}

void SonarDisplay::drawSonarGrid(Rectangle r) {
    Color gridColor = Fade(SKYBLUE, 0.15f);
    Vector2 center = {r.x + r.width/2.0f, r.y + r.height/2.0f};
    int maxRadius = static_cast<int>(static_cast<float>(std::min(r.width, r.height)) / 2.0f);

    // Enable scissor test to clip circles to sonar boundaries
    BeginScissorMode(static_cast<int>(r.x), static_cast<int>(r.y), static_cast<int>(r.width), static_cast<int>(r.height));
    
    for (int radius = 50; radius <= maxRadius + 100; radius += 50) {
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, gridColor);
    }
    
    EndScissorMode();
}

void SonarDisplay::drawCrossHair(Rectangle r) {
    // Draw cross-hair lines (bearing lines)
    Color gridColor = Fade(SKYBLUE, 0.15f); // Very subtle blue grid
    Vector2 center = {r.x + r.width/2.0f, r.y + r.height/2.0f};
    
    // Vertical line
    DrawLine(static_cast<int>(center.x), static_cast<int>(r.y), static_cast<int>(center.x), static_cast<int>(r.y + r.height), gridColor);
    
    // Horizontal line
    DrawLine(static_cast<int>(r.x), static_cast<int>(center.y), static_cast<int>(r.x + r.width), static_cast<int>(center.y), gridColor);
    
    // Diagonal lines (45-degree bearings)
    DrawLine(static_cast<int>(r.x), static_cast<int>(r.y), static_cast<int>(r.x + r.width), static_cast<int>(r.y + r.height), gridColor);
    DrawLine(static_cast<int>(r.x + r.width), static_cast<int>(r.y), static_cast<int>(r.x), static_cast<int>(r.y + r.height), gridColor);
}

void SonarDisplay::drawMouseReticle(Rectangle r) {
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, r)) {
        DrawCircleLines(static_cast<int>(m.x), static_cast<int>(m.y), 10, Fade(YELLOW, 0.5f));
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
