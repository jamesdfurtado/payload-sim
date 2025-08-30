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
    missile.speed = 80.0f;  // Fast missile speed
    missile.maxTurnRate = 3.0f;  // Radians per second turn rate
    missile.lifetime = 15.0f;  // 15 seconds max flight time
    missile.active = true;
    
    // Initial velocity in a random direction (will be corrected by heat-seeking)
    float randomAngle = ((float)GetRandomValue(0, 1000) / 1000.0f) * 2.0f * PI;
    missile.velocity = { cosf(randomAngle) * missile.speed, sinf(randomAngle) * missile.speed };
    
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

void MissileManager::explodeAllMissiles() {
    // Create explosions at each active missile position
    for (const auto& missile : activeMissiles) {
        if (missile.active) {
            createExplosion(missile.position);
        }
    }
    
    // Clear all missiles after creating explosions
    activeMissiles.clear();
}

bool MissileManager::isMissileActive(uint32_t id) const {
    for (const auto& missile : activeMissiles) {
        if (missile.id == id && missile.active) return true;
    }
    return false;
}

void MissileManager::updateMissilePhysics(float dt, const std::vector<Vector2>& targetPositions) {
    for (auto& missile : activeMissiles) {
        if (!missile.active) continue;
        
        // Update lifetime
        missile.lifetime -= dt;
        if (missile.lifetime <= 0.0f) {
            missile.active = false;
            continue;
        }
        
        // Find target position (assuming targetId corresponds to index in targetPositions)
        if (missile.targetId < targetPositions.size()) {
            Vector2 targetPos = targetPositions[missile.targetId];
            
            // Calculate heat-seeking velocity
            missile.velocity = calculateHeatSeekingVelocity(
                missile.position, missile.velocity, targetPos, missile.maxTurnRate, dt
            );
        } else {
            // Target is no longer in the list (left screen or was destroyed)
            // Continue on current trajectory - don't re-target
            // The missile will continue flying in its current direction
        }
        
        // Update position
        missile.position.x += missile.velocity.x * dt;
        missile.position.y += missile.velocity.y * dt;
        
        // Check if missile is out of bounds
        if (missile.position.x < -600 || missile.position.x > 600 || 
            missile.position.y < -360 || missile.position.y > 360) {
            missile.active = false;
        }
    }
    
    // Remove inactive missiles
    activeMissiles.erase(
        std::remove_if(activeMissiles.begin(), activeMissiles.end(), 
            [](const Missile& m) { return !m.active; }), 
        activeMissiles.end()
    );
}

void MissileManager::updateExplosions(float dt) {
    for (auto& explosion : activeExplosions) {
        if (!explosion.active) continue;
        
        explosion.timer += dt;
        float progress = explosion.timer / explosion.duration;
        
        // Update rings with different speeds for staggered effect
        explosion.innerRing = explosion.maxRingSize * std::min(1.0f, progress * 2.0f);     // Fast
        explosion.middleRing = explosion.maxRingSize * std::min(1.0f, progress * 1.5f);    // Medium
        explosion.outerRing = explosion.maxRingSize * progress;                            // Slow
        
        // Flash intensity: bright at start, fades quickly
        explosion.flashIntensity = std::max(0.0f, 1.0f - progress * 3.0f);
        
        if (explosion.timer >= explosion.duration) {
            explosion.active = false;
        }
    }
    
    // Remove inactive explosions
    activeExplosions.erase(
        std::remove_if(activeExplosions.begin(), activeExplosions.end(), 
            [](const Explosion& e) { return !e.active; }), 
        activeExplosions.end()
    );
}

void MissileManager::checkCollisions(const std::vector<Vector2>& contactPositions, std::vector<uint32_t>& hitContactIds) {
    hitContactIds.clear();
    
    for (auto& missile : activeMissiles) {
        if (!missile.active) continue;
        
        // Check collision with each contact
        for (size_t i = 0; i < contactPositions.size(); ++i) {
            Vector2 contactPos = contactPositions[i];
            float dx = missile.position.x - contactPos.x;
            float dy = missile.position.y - contactPos.y;
            float distance = sqrtf(dx*dx + dy*dy);
            
            // Collision radius (missile + contact)
            if (distance < 15.0f) {
                // Create explosion at missile position
                createExplosion(missile.position);
                
                // Mark missile as inactive
                missile.active = false;
                
                // Add contact to hit list (using index as ID)
                hitContactIds.push_back(static_cast<uint32_t>(i));
                
                break;  // Missile can only hit one target
            }
        }
    }
}

void MissileManager::createExplosion(Vector2 position) {
    Explosion explosion{};
    explosion.position = position;
    explosion.duration = 1.5f;  // 1.5 second explosion
    explosion.timer = 0.0f;
    explosion.active = true;
    
    // Initialize ring data
    explosion.innerRing = 0.0f;
    explosion.middleRing = 0.0f;
    explosion.outerRing = 0.0f;
    explosion.flashIntensity = 1.0f;
    explosion.maxRingSize = 60.0f;  // Larger explosion
    
    activeExplosions.push_back(explosion);
}

Vector2 MissileManager::calculateHeatSeekingVelocity(Vector2 missilePos, Vector2 missileVel, Vector2 targetPos, float maxTurnRate, float dt) {
    // Calculate desired direction to target
    Vector2 toTarget = { targetPos.x - missilePos.x, targetPos.y - missilePos.y };
    float targetDistance = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    
    if (targetDistance < 1.0f) {
        return missileVel;  // Already very close
    }
    
    // Normalize direction to target
    Vector2 desiredDir = { toTarget.x / targetDistance, toTarget.y / targetDistance };
    
    // Calculate current missile direction
    float currentSpeed = sqrtf(missileVel.x * missileVel.x + missileVel.y * missileVel.y);
    Vector2 currentDir = { missileVel.x / currentSpeed, missileVel.y / currentSpeed };
    
    // Calculate angle between current and desired direction
    float dotProduct = currentDir.x * desiredDir.x + currentDir.y * desiredDir.y;
    dotProduct = std::max(-1.0f, std::min(1.0f, dotProduct));  // Clamp to [-1, 1]
    float angleDiff = acosf(dotProduct);
    
    // Limit turn rate
    float maxAngleChange = maxTurnRate * dt;
    if (angleDiff > maxAngleChange) {
        angleDiff = maxAngleChange;
    }
    
    // Determine turn direction (cross product)
    float crossZ = currentDir.x * desiredDir.y - currentDir.y * desiredDir.x;
    if (crossZ < 0) angleDiff = -angleDiff;
    
    // Apply rotation to current velocity
    float cosAngle = cosf(angleDiff);
    float sinAngle = sinf(angleDiff);
    
    Vector2 newVel = {
        missileVel.x * cosAngle - missileVel.y * sinAngle,
        missileVel.x * sinAngle + missileVel.y * cosAngle
    };
    
    // Maintain speed
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
