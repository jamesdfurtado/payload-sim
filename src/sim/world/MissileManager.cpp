#include "MissileManager.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#ifndef PI
#define PI 3.14159265359f
#endif

MissileManager::MissileManager() {
    nextMissileId = 1;
}

uint32_t MissileManager::launchMissile(Vector2 startPosition, uint32_t targetId) {
    Missile missile{};
    missile.id = nextMissileId++;
    missile.position = startPosition;
    missile.targetId = targetId;
    missile.speed = 160.0f;
    missile.maxTurnRate = 3.0f;
    missile.lifetime = 15.0f;
    missile.active = true;
    
    // start missile in random direction-- then correct path
    float randomAngle = ((float)GetRandomValue(0, 1000) / 1000.0f) * 2.0f * PI;
    missile.velocity = { cosf(randomAngle) * missile.speed, sinf(randomAngle) * missile.speed };
    
    missile.trailPoints.clear();
    missile.trailPoints.push_back(missile.position);
    
    activeMissiles.push_back(missile);
    return missile.id;
}

void MissileManager::removeMissile(uint32_t id) {
    activeMissiles.erase(
        std::remove_if(activeMissiles.begin(), activeMissiles.end(), 
            [id](const Missile& m) { return m.id == id; }), 
        activeMissiles.end()
    );
}

void MissileManager::clearAllMissiles() {
    activeMissiles.clear();
    activeExplosions.clear();
}

// explodes all missiles
void MissileManager::explodeAllMissiles() {
    for (const auto& missile : activeMissiles) {
        if (missile.active) {
            createExplosion(missile.position);
        }
    }
    
    activeMissiles.clear();
}

bool MissileManager::isMissileActive(uint32_t id) const {
    for (const auto& missile : activeMissiles) {
        if (missile.id == id && missile.active) return true;
    }
    return false;
}

// updates missile frame-by-frame
void MissileManager::updateMissilePhysics(float dt, const std::vector<Vector2>& targetPositions) {
    for (auto& missile : activeMissiles) {
        if (!missile.active) continue;
        
        missile.lifetime -= dt;
        if (missile.lifetime <= 0.0f) {
            missile.active = false;
            continue;
        }
        
        // guide toward target
        if (missile.targetId < targetPositions.size()) {
            Vector2 targetPos = targetPositions[missile.targetId];
            
            missile.velocity = calculateHeatSeekingVelocity(
                missile.position, missile.velocity, targetPos, missile.maxTurnRate, dt
            );
        }
        
        Vector2 oldPosition = missile.position;
        missile.position.x += missile.velocity.x * dt;
        missile.position.y += missile.velocity.y * dt;
        
        missile.trailPoints.push_back(missile.position);
        
        // trail logic
        if (missile.trailPoints.size() > 20) {
            missile.trailPoints.erase(missile.trailPoints.begin());
        }
        
        if (missile.position.x < -600 || missile.position.x > 600 || 
            missile.position.y < -360 || missile.position.y > 360) {
            missile.active = false;
        }
    }
    
    activeMissiles.erase(
        std::remove_if(activeMissiles.begin(), activeMissiles.end(), 
            [](const Missile& m) { return !m.active; }), 
        activeMissiles.end()
    );
}

// explosion animation
void MissileManager::updateExplosions(float dt) {
    for (auto& explosion : activeExplosions) {
        if (!explosion.active) continue;
        
        explosion.timer += dt;
        float progress = explosion.timer / explosion.duration;
        
        // multiple rings during explosion
        explosion.innerRing = explosion.maxRingSize * std::min(1.0f, progress * 2.0f);
        explosion.middleRing = explosion.maxRingSize * std::min(1.0f, progress * 1.5f);
        explosion.outerRing = explosion.maxRingSize * progress;
        
        explosion.flashIntensity = std::max(0.0f, 1.0f - progress * 3.0f);
        
        if (explosion.timer >= explosion.duration) {
            explosion.active = false;
        }
    }
    
    // erase explosion
    activeExplosions.erase(
        std::remove_if(activeExplosions.begin(), activeExplosions.end(), 
            [](const Explosion& e) { return !e.active; }), 
        activeExplosions.end()
    );
}

// check collisions
void MissileManager::checkCollisions(const std::vector<Vector2>& contactPositions, std::vector<uint32_t>& hitContactIds) {
    hitContactIds.clear();
    
    for (auto& missile : activeMissiles) {
        if (!missile.active) continue;
        
        for (size_t i = 0; i < contactPositions.size(); ++i) {
            Vector2 contactPos = contactPositions[i];
            float dx = missile.position.x - contactPos.x;
            float dy = missile.position.y - contactPos.y;
            float distance = sqrtf(dx*dx + dy*dy);
            
            if (distance < 15.0f) {
                createExplosion(missile.position);
                
                missile.active = false;
                
                hitContactIds.push_back(static_cast<uint32_t>(i));
                
                break;
            }
        }
    }
}

void MissileManager::createExplosion(Vector2 position) {
    Explosion explosion{};
    explosion.position = position;
    explosion.duration = 1.5f;
    explosion.timer = 0.0f;
    explosion.active = true;
    
    explosion.innerRing = 0.0f;
    explosion.middleRing = 0.0f;
    explosion.outerRing = 0.0f;
    explosion.flashIntensity = 1.0f;
    explosion.maxRingSize = 60.0f;
    
    activeExplosions.push_back(explosion);
}

// heat seeking missile calculations
Vector2 MissileManager::calculateHeatSeekingVelocity(Vector2 missilePos, Vector2 missileVel, Vector2 targetPos, float maxTurnRate, float dt) {
    // vector from missile to target
    Vector2 toTarget = { targetPos.x - missilePos.x, targetPos.y - missilePos.y };
    float targetDistance = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

    if (targetDistance < 1.0f) {
        return missileVel;
    }

    // get desired direction towards target
    Vector2 desiredDir = { toTarget.x / targetDistance, toTarget.y / targetDistance };

    // get current speed and direction
    float currentSpeed = sqrtf(missileVel.x * missileVel.x + missileVel.y * missileVel.y);
    Vector2 currentDir = { missileVel.x / currentSpeed, missileVel.y / currentSpeed };

    // compute angle between current direction and desired direction
    float dotProduct = currentDir.x * desiredDir.x + currentDir.y * desiredDir.y;
    dotProduct = std::max(-1.0f, std::min(1.0f, dotProduct));  // Clamp for safety
    float angleDiff = acosf(dotProduct);

    // calculate turning radius
    float maxAngleChange = maxTurnRate * dt;
    if (angleDiff > maxAngleChange) {
        angleDiff = maxAngleChange;
    }

    // determine turn left or right
    float crossZ = currentDir.x * desiredDir.y - currentDir.y * desiredDir.x;
    if (crossZ < 0) angleDiff = -angleDiff;

    float cosAngle = cosf(angleDiff);
    float sinAngle = sinf(angleDiff);
    Vector2 newVel = {
        missileVel.x * cosAngle - missileVel.y * sinAngle,
        missileVel.x * sinAngle + missileVel.y * cosAngle
    };

    float newSpeed = sqrtf(newVel.x * newVel.x + newVel.y * newVel.y);
    if (newSpeed > 0) {
        newVel.x = (newVel.x / newSpeed) * currentSpeed;
        newVel.y = (newVel.y / newSpeed) * currentSpeed;
    }

    return newVel;
}

void MissileManager::updateMissileTargets(uint32_t newTargetId) {
    for (auto& missile : activeMissiles) {
        missile.targetId = newTargetId;
    }
}
