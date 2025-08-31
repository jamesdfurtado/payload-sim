#include <gtest/gtest.h>
#include "sim/world/MissileManager.h"
#include <cmath>

class MissileManagerTest : public ::testing::Test {
protected:
    MissileManager missileManager;
    
    void SetUp() override {
        // Initialize missile manager
    }
};

TEST_F(MissileManagerTest, InitializesEmpty) {
    EXPECT_TRUE(missileManager.getActiveMissiles().empty());
}

TEST_F(MissileManagerTest, MissileLaunch) {
    // Test missile launch functionality
    EXPECT_TRUE(missileManager.getActiveMissiles().empty());
    
    Vector2 launchPos = {100.0f, 200.0f};
    uint32_t targetId = 42;
    
    uint32_t missileId = missileManager.launchMissile(launchPos, targetId);
    
    // Should have created a missile
    EXPECT_GT(missileId, 0);
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 1);
    EXPECT_TRUE(missileManager.isMissileActive(missileId));
    
    // Check missile properties
    const auto& missiles = missileManager.getActiveMissiles();
    const auto& missile = missiles[0];
    EXPECT_EQ(missile.id, missileId);
    EXPECT_FLOAT_EQ(missile.position.x, launchPos.x);
    EXPECT_FLOAT_EQ(missile.position.y, launchPos.y);
    EXPECT_EQ(missile.targetId, targetId);
    EXPECT_TRUE(missile.active);
}

TEST_F(MissileManagerTest, MissileTrajectory) {
    // Test missile movement over time
    Vector2 launchPos = {0.0f, 0.0f};
    uint32_t targetId = 1;
    
    uint32_t missileId = missileManager.launchMissile(launchPos, targetId);
    
    // Get initial position
    Vector2 initialPos = missileManager.getActiveMissiles()[0].position;
    
    // Create target positions for missile to track
    std::vector<Vector2> targetPositions = {{100.0f, 100.0f}};
    
    // Update missile physics for several frames
    for (int i = 0; i < 60; i++) { // 1 second at 60fps
        missileManager.updateMissilePhysics(0.016f, targetPositions);
    }
    
    // Missile should have moved toward target
    const auto& missiles = missileManager.getActiveMissiles();
    if (!missiles.empty()) {
        Vector2 finalPos = missiles[0].position;
        
        // Should have moved from initial position
        float distance = sqrtf(powf(finalPos.x - initialPos.x, 2) + powf(finalPos.y - initialPos.y, 2));
        EXPECT_GT(distance, 0.1f);
        
        // Should be moving toward target (closer to target than before)
        float initialDistanceToTarget = sqrtf(powf(initialPos.x - 100.0f, 2) + powf(initialPos.y - 100.0f, 2));
        float finalDistanceToTarget = sqrtf(powf(finalPos.x - 100.0f, 2) + powf(finalPos.y - 100.0f, 2));
        EXPECT_LT(finalDistanceToTarget, initialDistanceToTarget);
    }
}

TEST_F(MissileManagerTest, MissileImpact) {
    // Test missile collision detection
    Vector2 launchPos = {0.0f, 0.0f};
    uint32_t missileId = missileManager.launchMissile(launchPos, 1);
    
    // Position contact very close to missile
    std::vector<Vector2> contactPositions = {{5.0f, 5.0f}}; // Within collision range
    std::vector<uint32_t> hitContactIds;
    
    missileManager.checkCollisions(contactPositions, hitContactIds);
    
    // Should detect collision
    EXPECT_EQ(hitContactIds.size(), 1);
    EXPECT_EQ(hitContactIds[0], 0); // Index of first contact
    
    // Missile should be deactivated after collision
    const auto& missiles = missileManager.getActiveMissiles();
    EXPECT_FALSE(missiles[0].active);
}

TEST_F(MissileManagerTest, TrailGeneration) {
    // Test missile trail generation over time
    Vector2 launchPos = {0.0f, 0.0f};
    uint32_t missileId = missileManager.launchMissile(launchPos, 1);
    
    // Initially should have no trail or minimal trail
    const auto& missiles = missileManager.getActiveMissiles();
    size_t initialTrailSize = missiles[0].trailPoints.size();
    
    // Update missile physics to generate trail
    std::vector<Vector2> targetPositions = {{100.0f, 100.0f}};
    for (int i = 0; i < 30; i++) { // Half second of movement
        missileManager.updateMissilePhysics(0.016f, targetPositions);
    }
    
    // Trail should have grown
    const auto& updatedMissiles = missileManager.getActiveMissiles();
    if (!updatedMissiles.empty()) {
        size_t finalTrailSize = updatedMissiles[0].trailPoints.size();
        EXPECT_GT(finalTrailSize, initialTrailSize);
        
        // Trail points should represent the missile's movement path
        if (finalTrailSize > 1) {
            // Later trail points should be closer to the target
            Vector2 firstTrailPoint = updatedMissiles[0].trailPoints[0];
            Vector2 lastTrailPoint = updatedMissiles[0].trailPoints[finalTrailSize - 1];
            
            float firstDistance = sqrtf(powf(firstTrailPoint.x - 100.0f, 2) + powf(firstTrailPoint.y - 100.0f, 2));
            float lastDistance = sqrtf(powf(lastTrailPoint.x - 100.0f, 2) + powf(lastTrailPoint.y - 100.0f, 2));
            
            EXPECT_LT(lastDistance, firstDistance);
        }
    }
}

TEST_F(MissileManagerTest, ExplosionHandling) {
    // Test explosion creation and timing
    Vector2 launchPos = {0.0f, 0.0f};
    uint32_t missileId = missileManager.launchMissile(launchPos, 1);
    
    // Initially no explosions
    EXPECT_TRUE(missileManager.getActiveExplosions().empty());
    
    // Explode all missiles
    missileManager.explodeAllMissiles();
    
    // Should have created an explosion
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 1);
    
    const auto& explosions = missileManager.getActiveExplosions();
    const auto& explosion = explosions[0];
    
    // Explosion should be active and at missile position
    EXPECT_TRUE(explosion.active);
    EXPECT_FLOAT_EQ(explosion.position.x, launchPos.x);
    EXPECT_FLOAT_EQ(explosion.position.y, launchPos.y);
    EXPECT_GT(explosion.duration, 0.0f);
    EXPECT_FLOAT_EQ(explosion.timer, 0.0f);
    
    // Update explosions over time
    for (int i = 0; i < 100; i++) { // Update for ~1.6 seconds
        missileManager.updateExplosions(0.016f);
    }
    
    // Explosion should eventually finish and be removed
    const auto& updatedExplosions = missileManager.getActiveExplosions();
    // Either explosion is inactive or removed entirely
    if (!updatedExplosions.empty()) {
        EXPECT_FALSE(updatedExplosions[0].active);
    }
}
