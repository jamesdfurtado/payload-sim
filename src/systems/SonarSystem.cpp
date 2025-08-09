#include "SonarSystem.h"
#include <cmath>
#include <raymath.h>

static float rand01() {
    return (float) GetRandomValue(0, 10000) / 10000.0f;
}

// Spawn the little dot "submarines"
void SonarSystem::spawnContact() {
    SonarContact c{};
    c.position = { (float)GetRandomValue(-500, 500), (float)GetRandomValue(-300, 300) };
    c.velocityDirRad = rand01() * 2.0f * PI;
    c.speed = 10.0f + rand01() * 20.0f;
    float r = rand01();
    if (r < 0.25f) c.type = ContactType::EnemySub;
    else if (r < 0.40f) c.type = ContactType::FriendlySub;
    else if (r < 0.80f) c.type = ContactType::Fish;
    else c.type = ContactType::Debris;
    contacts.push_back(c);
}

void SonarSystem::update(SimulationState& state, float dt) {
    spawnTimer -= dt;
    
    // Ensure board is populated at al times
    while (contacts.size() < 10) {
        spawnContact();
    }
    if (spawnTimer <= 0.0f && contacts.size() < 20) {
        spawnContact();
        spawnTimer = 1.5f + rand01() * 2.0f;
    }

    // Update contact positions, remove if out of bounds
    for (auto it = contacts.begin(); it != contacts.end(); ) {
        int idx = static_cast<int>(it - contacts.begin());
        it->position.x += cosf(it->velocityDirRad) * it->speed * dt;
        it->position.y += sinf(it->velocityDirRad) * it->speed * dt;
        if (it->position.x < -600 || it->position.x > 600 || it->position.y < -360 || it->position.y > 360) {
            // If contact was removed and locked, unlock crosshair
            if (targetAcquired && lockedTargetIdx == idx) {
                targetAcquired = false;
                targetValidated = false;
                lockedTargetIdx = -1;
            } else if (targetAcquired && lockedTargetIdx > idx) {
                lockedTargetIdx--;
            }
            it = contacts.erase(it);
        } else {
            ++it;
        }
    }

    // Continuously lock on to target if tracked
    if (targetAcquired && lockedTargetIdx >= 0 && lockedTargetIdx < (int)contacts.size()) {
        lockedTarget = contacts[lockedTargetIdx].position;
        if (contacts[lockedTargetIdx].type == ContactType::EnemySub) {
            targetValidated = true;
        } else {
            targetValidated = false;
        }
    } else if (targetAcquired) {
        targetAcquired = false;
        targetValidated = false;
        lockedTargetIdx = -1;
    }

    state.targetValidated = targetValidated;
    state.targetAcquired = targetAcquired;

    // Ensure no friendlies in blast radius
    if (targetAcquired) {
        float blastRadius = 80.0f;
        bool friendlyInside = false;
        for (auto& c : contacts) {
            if (c.type == ContactType::FriendlySub) {
                if (Vector2Distance(c.position, lockedTarget) < blastRadius) {
                    friendlyInside = true;
                    break;
                }
            }
        }
        if (Vector2Distance({0,0}, lockedTarget) < blastRadius) {
            friendlyInside = true;
        }
        state.noFriendlyUnitsInBlastRadius = !friendlyInside;
    }
}

void SonarSystem::attemptManualLock(const Vector2& cursorWorldPos) {
    // Find nearest contact to cursor position
    float nearest = 99999.0f;
    int bestIdx = -1;
    for (int i = 0; i < (int)contacts.size(); ++i) {
        float d = Vector2Distance(contacts[i].position, cursorWorldPos);
        if (d < nearest) { nearest = d; bestIdx = i; }
    }
    if (bestIdx >= 0 && nearest < 25.0f) {
        lockedTargetIdx = bestIdx;
        lockedTarget = contacts[bestIdx].position;
        targetAcquired = true;
        if (contacts[bestIdx].type == ContactType::EnemySub) {
            targetValidated = true;
        } else {
            targetValidated = false;
        }
    }
}

void SonarSystem::removeContact(int idx) {
    if (idx >= 0 && idx < (int)contacts.size()) {
        // If removing the currently locked target, disengage crosshair
        if (targetAcquired && lockedTargetIdx == idx) {
            targetAcquired = false;
            targetValidated = false;
            lockedTargetIdx = -1;
        } else if (targetAcquired && lockedTargetIdx > idx) {
            // Adjust locked target index if it's after the removed contact
            lockedTargetIdx--;
        }
        
        contacts.erase(contacts.begin() + idx);
    }
}


