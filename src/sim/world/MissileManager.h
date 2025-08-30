#pragma once

#include <vector>
#include <cstdint>
#include <raylib.h>

struct Missile {
    uint32_t id;
    Vector2 position;
    Vector2 velocity;
    uint32_t targetId;
    float speed;
    float maxTurnRate;
    float lifetime;
    bool active;
    
    // Trail data - stores recent positions to create a path trail
    std::vector<Vector2> trailPoints;
    static constexpr float MAX_TRAIL_LENGTH = 60.0f;  // Maximum trail length in world units
};

struct Explosion {
    Vector2 position;
    float duration;
    float timer;
    bool active;
    
    // Multi-ring explosion data
    float innerRing;
    float middleRing;
    float outerRing;
    float flashIntensity;
    float maxRingSize;
};

class MissileManager {
public:
    MissileManager();

    uint32_t launchMissile(Vector2 startPosition, uint32_t targetId);
    void removeMissile(uint32_t id);
    void clearAllMissiles();
    void explodeAllMissiles();  // Creates explosions at missile positions before clearing

    const std::vector<Missile>& getActiveMissiles() const { return activeMissiles; }
    const std::vector<Explosion>& getActiveExplosions() const { return activeExplosions; }
    bool isMissileActive(uint32_t id) const;

    void updateMissilePhysics(float dt, const std::vector<Vector2>& targetPositions);
    void updateExplosions(float dt);
    void checkCollisions(const std::vector<Vector2>& contactPositions, std::vector<uint32_t>& hitContactIds);
    void updateMissileTargets(uint32_t newTargetId);

private:
    std::vector<Missile> activeMissiles;
    std::vector<Explosion> activeExplosions;
    uint32_t nextMissileId = 1;
    
    void createExplosion(Vector2 position);
    Vector2 calculateHeatSeekingVelocity(Vector2 missilePos, Vector2 missileVel, Vector2 targetPos, float maxTurnRate, float dt);
};
