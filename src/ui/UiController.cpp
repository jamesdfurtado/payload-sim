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
    // Handle RESETTING... state
    if (resetTimer > 0.0f && safety && safety->getPhase() == LaunchPhase::Resetting) {
        resetTimer -= dt;
        if (resetTimer <= 0.0f) {
            // Return to IDLE
            if (safety) safety->forceIdle();
            feedbackMessage = "System ready.";
        }
        return;
    }

    // Mouse click handling for AUTH code box
    Rectangle controls = { 640, 380, 620, 320 };
    Rectangle authBox = { controls.x + 10, controls.y + 280, 300, 28 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, authBox)) {
            authBoxFocused = true;
        } else {
            authBoxFocused = false;
        }
    }

    // AUTH code entry when focused
    if (authBoxFocused) {
        if (IsKeyPressed(KEY_BACKSPACE) && !authCodeInput.empty()) {
            authCodeInput.pop_back();
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            authBoxFocused = false;
            feedbackMessage = "Authorization cancelled.";
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (safety) {
                if (authCodeInput == authChallengeCode) {
                    // Check all conditions except Authorization
                    auto& s = engine.getState();
                    bool allOtherConditions = s.targetValidated && s.targetAcquired && s.depthClearanceMet && s.launchTubeIntegrity && s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
                    if (allOtherConditions) {
                        safety->requestAuthorizationCode(authCodeInput.c_str());
                        feedbackMessage = "Authorization granted, please ARM the payload.";
                    } else {
                        feedbackMessage = "Cannot authorize: All conditions (except Authorization) must be met.";
                    }
                } else {
                    feedbackMessage = "Incorrect AUTH code.";
                }
            }
            authBoxFocused = false;
            return;
        }
        int key = GetCharPressed();
        while (key > 0) {
            if (key < 128 && std::isprint(key)) {
                authCodeInput += (char)key;
            }
            key = GetCharPressed();
        }
        return;
    }

    // Normal controls
    float ballast = 0.5f;
    if (IsKeyDown(KEY_W)) ballast = 1.0f;
    else if (IsKeyDown(KEY_S)) ballast = 0.0f;
    if (depth) depth->setBallastThrottle(ballast);

    if (IsKeyDown(KEY_A)) uiWeaponsPower = std::max(0.0f, uiWeaponsPower - 0.3f * dt);
    if (IsKeyDown(KEY_D)) uiWeaponsPower = std::min(1.0f, uiWeaponsPower + 0.3f * dt);
    if (power) power->routePowerToWeapons(uiWeaponsPower);

    if (IsKeyDown(KEY_Q)) { if (targeting) targeting->adjustStability(-0.4f * dt); }
    if (IsKeyDown(KEY_E)) { if (targeting) targeting->adjustStability(+0.4f * dt); }

    // Process AUTH request
    if (IsKeyPressed(KEY_ONE)) {
        if (!authBoxFocused) {
            // Check all conditions except Authorization
            auto& s = engine.getState();
            bool allOtherConditions = s.targetValidated && s.targetAcquired && s.depthClearanceMet && s.launchTubeIntegrity && s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
            if (allOtherConditions) {
                authBoxFocused = true;
                // Generate random 4-digit code
                int code = 1000 + std::rand() % 9000;
                authChallengeCode = std::to_string(code);
                if (safety) safety->setChallengeCode(authChallengeCode); // Set challenge code in SafetySystem
                authCodeInput.clear();
                feedbackMessage = "Enter the AUTH code shown below.";
            } else {
                feedbackMessage = "Cannot request AUTH: All conditions (except Authorization) must be met.";
                authBoxFocused = false;
            }
        } else {
            // If code entered, pressing "1" again will submit
            if (safety) {
                if (authCodeInput == authChallengeCode) {
                    // Double check conditions for safety
                    auto& s = engine.getState();
                    bool allOtherConditions = s.targetValidated && s.targetAcquired && s.depthClearanceMet && s.launchTubeIntegrity && s.powerSupplyStable && s.noFriendlyUnitsInBlastRadius && s.launchConditionsFavorable;
                    if (allOtherConditions) {
                        safety->requestAuthorizationCode(authCodeInput.c_str());
                        feedbackMessage = "Authorization granted, please ARM the payload.";
                    } else {
                        feedbackMessage = "Cannot authorize: All conditions (except Authorization) must be met.";
                    }
                } else {
                    feedbackMessage = "Incorrect AUTH code.";
                }
            }
            authBoxFocused = false;
        }
        return;
    }

    // Process ARM request
    if (IsKeyPressed(KEY_TWO) && safety) {
        if (safety->getPhase() != LaunchPhase::Authorized) {
            feedbackMessage = "Cannot ARM: Not authorized.";
        } else {
            safety->arm();
            feedbackMessage.clear();
        }
    }
    if (IsKeyPressed(KEY_THREE) && safety) {
        if (safety->getPhase() != LaunchPhase::Armed) {
            feedbackMessage = "Cannot LAUNCH: Not ARMED.";
        } else {
            safety->launch();
        }
    }
    if (IsKeyPressed(KEY_FOUR) && safety) {
        safety->reset();
        feedbackMessage = "System RESETTING...";
        resetTimer = 2.0f;
        return;
    }

    if (sonar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle sonarRect{ 20, 120, 600, 580 };
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, sonarRect)) {
            Vector2 world = screenToWorld(m, sonarRect);
            sonar->attemptManualLock(world);
        }
    }

    // Missile launch animation
    if (missileActive) {
        if (missileExplosionTimer > 0.0f) {
            missileExplosionTimer -= dt;
            if (missileExplosionTimer <= 0.0f && sonar && missileTargetIdx >= 0) {
                // Remove enemy and disengage lock
                sonar->removeContact(missileTargetIdx);
                missileActive = false;
            }
        } else {
            // Track current enemy position
            Vector2 targetWorld = {0,0};
            if (sonar && missileTargetIdx >= 0) {
                const auto& contacts = sonar->getContacts();
                if (missileTargetIdx < (int)contacts.size() && contacts[missileTargetIdx].type == ContactType::EnemySub) {
                    targetWorld = contacts[missileTargetIdx].position;
                } else {
                    // Target gone, just finish at last known
                    targetWorld = screenToWorld(missileTarget, {20, 120, 600, 580});
                }
            }
            Rectangle sonarR = {20, 120, 600, 580};
            missileTarget = worldToScreen(targetWorld, sonarR);
            float speed = MISSILE_SPEED;
            Vector2 toTarget = {missileTarget.x - missilePos.x, missileTarget.y - missilePos.y};
            float dist = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
            if (dist > 1.0f) {
                missileVel = {toTarget.x / dist * speed * dt, toTarget.y / dist * speed * dt};
                missilePos.x += missileVel.x;
                missilePos.y += missileVel.y;
                missileTrail.push_back(missilePos);
                if (missileTrail.size() > 40) missileTrail.erase(missileTrail.begin());
                if (dist < 16.0f) {
                    missilePos = missileTarget;
                    missileExplosionTimer = EXPLOSION_DURATION;
                }
            } else {
                missilePos = missileTarget;
                missileExplosionTimer = EXPLOSION_DURATION;
            }
        }
    }
    // Missile launch logic
    static bool prevLaunched = false;
    bool nowLaunched = (safety && safety->getPhase() == LaunchPhase::Launched);
    if (nowLaunched && !prevLaunched && sonar && sonar->isTargetAcquired()) {
        Rectangle sonarR = {20, 120, 600, 580};
        missileActive = true;
        missileTrail.clear();
        missilePos = worldToScreen({0,0}, sonarR); // launch from sub center
        missileTarget = worldToScreen(sonar->getLockedTarget(), sonarR);
        missileProgress = 0.0f;
        missileExplosionTimer = 0.0f;
        // Actively heat-seek to enemy location
        const auto& contacts = sonar->getContacts();
        missileTargetIdx = -1;
        for (int i = 0; i < (int)contacts.size(); ++i) {
            if (contacts[i].type == ContactType::EnemySub && fabsf(contacts[i].position.x - sonar->getLockedTarget().x) < 1e-2f && fabsf(contacts[i].position.y - sonar->getLockedTarget().y) < 1e-2f) {
                missileTargetIdx = i;
                break;
            }
        }
    }
    prevLaunched = nowLaunched;
}

void UiController::render() {
    auto& s = engine.getState();

    Rectangle status = { 640, 20, 620, 110 };
    Rectangle powerR = { 640, 140, 620, 110 };
    Rectangle depthR = { 640, 260, 620, 110 };
    Rectangle controls = { 640, 380, 620, 320 };
    Rectangle sonarR = { 20, 120, 600, 580 };

    DrawText("Submarine Payload Launch Simulator", 20, 20, 24, RAYWHITE);

    drawStatusLights(s, status);
    drawPower(powerR);
    drawDepth(depthR);
    drawControls(controls);
    drawSonar(sonarR);
}

void UiController::drawStatusLights(const SimulationState& s, Rectangle r) {
    DrawRectangleRec(r, Fade(DARKGRAY, 0.4f));
    const int pad = 10;
    const int boxW = 150;
    const int boxH = 28;
    struct Item { const char* label; bool ok; } items[] = {
        {"Authorization", s.canLaunchAuthorized},
        {"Target Validated", s.targetValidated},
        {"Target Acquired", s.targetAcquired},
        {"Depth OK", s.depthClearanceMet},
        {"Tube Integrity", s.launchTubeIntegrity},
        {"Payload OK", s.payloadSystemOperational},
        {"Power Stable", s.powerSupplyStable},
        {"No Friendlies", s.noFriendlyUnitsInBlastRadius},
        {"Conditions OK", s.launchConditionsFavorable},
    };
    int x = (int)r.x + pad, y = (int)r.y + pad;
    for (auto& it : items) {
        Color c = it.ok ? GREEN : RED;
        DrawRectangleLines(x, y, boxW, boxH, LIGHTGRAY);
        DrawCircle(x + 12, y + boxH/2, 8, c);
        DrawText(it.label, x + 28, y + 6, 16, RAYWHITE);
        x += boxW + pad;
        if (x + boxW > r.x + r.width) { x = (int)r.x + pad; y += boxH + pad; }
    }
}

void UiController::drawPower(Rectangle r) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKBLUE, 0.3f));
    DrawText("Power", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);
    DrawText(TextFormat("Battery: %.1f%%", s.batteryPercent), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Weapons Power: %.0f%%", uiWeaponsPower * 100.0f), (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
}

void UiController::drawDepth(Rectangle r) {
    auto& s = engine.getState();
    DrawRectangleRec(r, Fade(DARKGREEN, 0.3f));
    DrawText("Depth", (int)r.x + 10, (int)r.y + 8, 20, LIME);
    DrawText(TextFormat("Depth: %.1fm", s.submarineDepthMeters), (int)r.x + 10, (int)r.y + 40, 18, RAYWHITE);
    DrawText(TextFormat("Optimal: %.1fm", s.optimalLaunchDepthMeters), (int)r.x + 10, (int)r.y + 64, 18, RAYWHITE);
}

void UiController::drawControls(Rectangle r) {
    DrawRectangleRec(r, Fade(DARKBROWN, 0.3f));
    DrawText("Controls", (int)r.x + 10, (int)r.y + 8, 20, ORANGE);
    DrawText("W/S ballast | A/D power | Q/E stabilize | Click sonar to lock", (int)r.x + 10, (int)r.y + 40, 18, GRAY);
    DrawText("1) Request AUTH", (int)r.x + 10, (int)r.y + 70, 18, RAYWHITE);
    DrawText("2) ARM", (int)r.x + 10, (int)r.y + 94, 18, RAYWHITE);
    DrawText("3) LAUNCH", (int)r.x + 10, (int)r.y + 118, 18, RAYWHITE);
    DrawText("4) RESET", (int)r.x + 10, (int)r.y + 142, 18, RAYWHITE);
    // Show current launch phase
    if (safety) {
        const char* phaseStr = "";
        switch (safety->getPhase()) {
            case LaunchPhase::Idle: phaseStr = "IDLE"; break;
            case LaunchPhase::Authorized: phaseStr = "AUTHORIZED"; break;
            case LaunchPhase::Arming: phaseStr = "ARMING"; break;
            case LaunchPhase::Armed: phaseStr = "ARMED"; break;
            case LaunchPhase::Launching: phaseStr = "LAUNCHING"; break;
            case LaunchPhase::Launched: phaseStr = "LAUNCHED"; break;
            case LaunchPhase::Resetting: phaseStr = (resetTimer > 0.0f ? "RESETTING..." : "RESET"); break;
        }
        DrawText(TextFormat("Payload State: %s", phaseStr), (int)r.x + 10, (int)r.y + 170, 20, YELLOW);
    }
    // Show feedback message or override for transition states
    if (safety) {
        switch (safety->getPhase()) {
            case LaunchPhase::Arming:
                DrawText("Arming payload...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Launching:
                DrawText("Launching payload...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Launched:
                DrawText("Payload LAUNCHED!", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                break;
            case LaunchPhase::Resetting:
                if (!safety->getResetReason().empty()) {
                    DrawText(TextFormat("System resetting: %s", safety->getResetReason().c_str()), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                } else {
                    DrawText("System RESETTING...", (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                }
                break;
            default:
                if (!feedbackMessage.empty()) {
                    DrawText(feedbackMessage.c_str(), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
                }
                break;
        }
    } else if (!feedbackMessage.empty()) {
        DrawText(feedbackMessage.c_str(), (int)r.x + 10, (int)r.y + 200, 18, LIGHTGRAY);
    }
    // Draw AUTH code label and text box further down
    Rectangle authBox = { r.x + 10, r.y + 280, 300, 28 };
    DrawText("AUTH CODE:", (int)authBox.x, (int)authBox.y - 22, 18, SKYBLUE);
    DrawRectangleLinesEx(authBox, 2, authBoxFocused ? YELLOW : GRAY);
    DrawText(authCodeInput.c_str(), (int)authBox.x + 8, (int)authBox.y + 4, 18, SKYBLUE);
    if (authBoxFocused) {
        DrawRectangle((int)authBox.x + 8 + MeasureText(authCodeInput.c_str(), 18), (int)authBox.y + 6, 12, 18, SKYBLUE);
    }
    // Show AUTH challenge code
    if (authBoxFocused && !authChallengeCode.empty()) {
        int boxW = 220, boxH = 60;
        int boxX = (int)authBox.x + 320;
        int boxY = (int)authBox.y - 10;
        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.85f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, SKYBLUE);
        DrawText("AUTH CHALLENGE", boxX + 16, boxY + 8, 18, SKYBLUE);
        DrawText(authChallengeCode.c_str(), boxX + 60, boxY + 32, 28, YELLOW);
    }
}

void UiController::drawSonar(Rectangle r) {
    DrawRectangleRec(r, Fade(BLUE, 0.2f));
    DrawRectangleLinesEx(r, 1, SKYBLUE);
    DrawText("Sonar", (int)r.x + 10, (int)r.y + 8, 20, SKYBLUE);

    // Draw submarine icon at center
    Vector2 subCenter = worldToScreen({0,0}, r);
    DrawRectangle((int)subCenter.x - 10, (int)subCenter.y - 6, 20, 12, DARKGRAY);
    DrawTriangle(
        Vector2{subCenter.x + 10.0f, subCenter.y},
        Vector2{subCenter.x + 18.0f, subCenter.y - 8.0f},
        Vector2{subCenter.x + 18.0f, subCenter.y + 8.0f},
        GRAY);
    DrawCircle((int)subCenter.x - 8, (int)subCenter.y, 4, SKYBLUE);

    if (sonar) {
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
        // Draw locked target
        if (engine.getState().targetAcquired) {
            Vector2 sp = worldToScreen(sonar->getLockedTarget(), r);
            DrawCircleLines((int)sp.x, (int)sp.y, 12, YELLOW);
            DrawLine((int)sp.x - 16, (int)sp.y, (int)sp.x + 16, (int)sp.y, YELLOW);
            DrawLine((int)sp.x, (int)sp.y - 16, (int)sp.x, (int)sp.y + 16, YELLOW);
        }
    }
    // Draw missile and trail
    if (missileActive) {
        // Trail
        for (size_t i = 1; i < missileTrail.size(); ++i) {
            DrawLine((int)missileTrail[i-1].x, (int)missileTrail[i-1].y, (int)missileTrail[i].x, (int)missileTrail[i].y, Fade(GRAY, 0.5f));
        }
        // Missile
        DrawCircle((int)missilePos.x, (int)missilePos.y, 6, ORANGE);
        // Explosion
        if (missileExplosionTimer > 0.0f) {
            float t = 1.0f - missileExplosionTimer / EXPLOSION_DURATION;
            DrawCircle((int)missilePos.x, (int)missilePos.y, 24 * t, YELLOW);
            DrawCircle((int)missilePos.x, (int)missilePos.y, 12 * t, RED);
        }
    }
    // Mouse reticle
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, r)) {
        DrawCircleLines((int)m.x, (int)m.y, 10, Fade(YELLOW, 0.5f));
    }
}

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


