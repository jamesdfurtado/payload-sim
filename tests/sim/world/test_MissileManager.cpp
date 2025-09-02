#include <gtest/gtest.h>
#include "sim/world/MissileManager.h"

class MissileManagerTest : public ::testing::Test {
protected:
    MissileManager missileManager;
    
    void SetUp() override {
    }
};

TEST_F(MissileManagerTest, InitializesEmpty) {
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 0);
}

TEST_F(MissileManagerTest, LaunchesMissileWithValidId) {
    uint32_t missileId = missileManager.launchMissile({0.0f, 0.0f}, 1);
    
    EXPECT_GT(missileId, 0);
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 1);
    EXPECT_TRUE(missileManager.isMissileActive(missileId));
}

TEST_F(MissileManagerTest, LaunchedMissileHasCorrectProperties) {
    uint32_t targetId = 5;
    Vector2 startPos = {100.0f, 200.0f};
    uint32_t missileId = missileManager.launchMissile(startPos, targetId);
    
    const auto& missiles = missileManager.getActiveMissiles();
    ASSERT_EQ(missiles.size(), 1);
    
    const auto& missile = missiles[0];
    EXPECT_EQ(missile.id, missileId);
    EXPECT_EQ(missile.position.x, startPos.x);
    EXPECT_EQ(missile.position.y, startPos.y);
    EXPECT_EQ(missile.targetId, targetId);
    EXPECT_TRUE(missile.active);
    EXPECT_GT(missile.lifetime, 0.0f);
}

TEST_F(MissileManagerTest, RemovesMissileById) {
    uint32_t missileId = missileManager.launchMissile({0.0f, 0.0f}, 1);
    EXPECT_TRUE(missileManager.isMissileActive(missileId));
    
    missileManager.removeMissile(missileId);
    
    EXPECT_FALSE(missileManager.isMissileActive(missileId));
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
}

TEST_F(MissileManagerTest, ClearsAllMissilesAndExplosions) {
    missileManager.launchMissile({0.0f, 0.0f}, 1);
    missileManager.launchMissile({10.0f, 10.0f}, 2);
    missileManager.explodeAllMissiles();
    
    EXPECT_GT(missileManager.getActiveExplosions().size(), 0);
    
    missileManager.clearAllMissiles();
    
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 0);
}

TEST_F(MissileManagerTest, ExplodesAllActiveMissiles) {
    missileManager.launchMissile({0.0f, 0.0f}, 1);
    missileManager.launchMissile({50.0f, 50.0f}, 2);
    
    missileManager.explodeAllMissiles();
    
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 2);
}

TEST_F(MissileManagerTest, UpdatesMissilePhysics) {
    uint32_t missileId = missileManager.launchMissile({0.0f, 0.0f}, 0);
    std::vector<Vector2> targets = {{100.0f, 100.0f}};
    
    Vector2 initialPos = missileManager.getActiveMissiles()[0].position;
    
    missileManager.updateMissilePhysics(0.1f, targets);
    
    Vector2 newPos = missileManager.getActiveMissiles()[0].position;
    EXPECT_NE(initialPos.x, newPos.x);
    EXPECT_NE(initialPos.y, newPos.y);
}

TEST_F(MissileManagerTest, RemovesExpiredMissiles) {
    uint32_t missileId = missileManager.launchMissile({0.0f, 0.0f}, 0);
    std::vector<Vector2> targets = {{100.0f, 100.0f}};
    
    for (int i = 0; i < 200; i++) {
        missileManager.updateMissilePhysics(0.1f, targets);
    }
    
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
}

TEST_F(MissileManagerTest, DetectsCollisions) {
    uint32_t missileId = missileManager.launchMissile({0.0f, 0.0f}, 0);
    std::vector<Vector2> contactPositions = {{5.0f, 5.0f}};
    std::vector<uint32_t> hitContactIds;
    
    missileManager.checkCollisions(contactPositions, hitContactIds);
    
    EXPECT_EQ(hitContactIds.size(), 1);
    EXPECT_EQ(hitContactIds[0], 0);
    EXPECT_GT(missileManager.getActiveExplosions().size(), 0);
}

TEST_F(MissileManagerTest, UpdatesExplosionAnimations) {
    missileManager.launchMissile({0.0f, 0.0f}, 0);
    missileManager.explodeAllMissiles();
    
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 1);
    
    for (int i = 0; i < 200; i++) {
        missileManager.updateExplosions(0.01f);
    }
    
    EXPECT_EQ(missileManager.getActiveExplosions().size(), 0);
}

TEST_F(MissileManagerTest, UpdatesMissileTargets) {
    missileManager.launchMissile({0.0f, 0.0f}, 1);
    missileManager.launchMissile({10.0f, 10.0f}, 2);
    
    missileManager.updateMissileTargets(5);
    
    const auto& missiles = missileManager.getActiveMissiles();
    for (const auto& missile : missiles) {
        EXPECT_EQ(missile.targetId, 5);
    }
}

TEST_F(MissileManagerTest, HandlesOutOfBoundsMissiles) {
    uint32_t missileId = missileManager.launchMissile({-700.0f, 0.0f}, 0);
    std::vector<Vector2> targets = {{-800.0f, 0.0f}};
    
    missileManager.updateMissilePhysics(1.0f, targets);
    
    EXPECT_EQ(missileManager.getActiveMissiles().size(), 0);
}
