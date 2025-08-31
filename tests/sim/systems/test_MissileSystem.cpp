#include <gtest/gtest.h>
#include "sim/systems/MissileSystem.h"
#include "sim/SimulationState.h"
#include "sim/world/MissileManager.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"
#include <raylib.h>

class MissileSystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<MissileManager> missileManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    std::unique_ptr<MissileSystem> missileSystem;
    SimulationState state;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        missileManager = std::make_unique<MissileManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        missileSystem = std::make_unique<MissileSystem>(*missileManager, *contactManager, *crosshairManager);
        
        // Initialize state
        state.missileLaunched = false;
        state.missileActive = false;
        state.explosionActive = false;
        state.targetAcquired = false;
        state.missileTargetId = 0;
    }
};

TEST_F(MissileSystemTest, InitializesCorrectly) {
    EXPECT_STREQ(missileSystem->getName(), "MissileSystem");
}

TEST_F(MissileSystemTest, RequiresTargetForLaunch) {
    // Test that missile launch requires a target to be acquired
    state.missileLaunched = true;
    state.targetAcquired = false;
    
    missileSystem->update(state, 0.016f);
    
    // Missile should not be active without a target
    EXPECT_FALSE(state.missileActive);
}

TEST_F(MissileSystemTest, LaunchesWithValidTarget) {
    // Spawn an enemy contact
    uint32_t contactId = contactManager->spawnContact();
    
    // Select the target with crosshair
    Vector2 targetPos = contactManager->getActiveContacts()[0].position;
    crosshairManager->setTargetingCirclePosition(targetPos);
    crosshairManager->trackContact(contactId);
    
    // Set required conditions
    state.targetAcquired = true;
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    // Missile should now be active
    EXPECT_TRUE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, contactId);
    EXPECT_TRUE(missileManager->isMissileActive());
}

TEST_F(MissileSystemTest, MissileExplodesOnTargetLoss) {
    // Setup: launch a missile with a valid target
    uint32_t contactId = contactManager->spawnContact();
    crosshairManager->trackContact(contactId);
    state.targetAcquired = true;
    state.missileLaunched = true;
    state.missileActive = true;
    state.missileTargetId = contactId;
    
    // Remove the contact (target destroyed)
    contactManager->removeContact(contactId);
    
    missileSystem->update(state, 0.016f);
    
    // Missile should explode and become inactive
    EXPECT_FALSE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, 0);
    EXPECT_FALSE(missileManager->isMissileActive());
}

TEST_F(MissileSystemTest, MissilePhysicsUpdate) {
    // Setup: active missile with target
    uint32_t contactId = contactManager->spawnContact();
    crosshairManager->trackContact(contactId);
    state.targetAcquired = true;
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    // Missile should be active
    EXPECT_TRUE(state.missileActive);
    
    // Update multiple frames - missile should continue tracking
    for (int i = 0; i < 10; i++) {
        contactManager->updateContactPositions(0.016f);
        missileSystem->update(state, 0.016f);
        
        // Missile should remain active while target exists
        EXPECT_TRUE(state.missileActive);
    }
}

TEST_F(MissileSystemTest, HandlesMultipleContactScenario) {
    // Spawn multiple contacts
    uint32_t enemy1 = contactManager->spawnContact();
    uint32_t enemy2 = contactManager->spawnContact();
    uint32_t enemy3 = contactManager->spawnContact();
    
    // Target the second enemy
    crosshairManager->trackContact(enemy2);
    state.targetAcquired = true;
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    // Should target the correct enemy
    EXPECT_TRUE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, enemy2);
    
    // Remove other contacts - missile should still track enemy2
    contactManager->removeContact(enemy1);
    contactManager->removeContact(enemy3);
    
    missileSystem->update(state, 0.016f);
    
    // Missile should still be active, targeting enemy2
    EXPECT_TRUE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, enemy2);
}

TEST_F(MissileSystemTest, ExplosionManagerIntegration) {
    // Setup missile with target
    uint32_t contactId = contactManager->spawnContact();
    crosshairManager->trackContact(contactId);
    state.targetAcquired = true;
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    // Force missile explosion by removing target
    contactManager->removeContact(contactId);
    missileSystem->update(state, 0.016f);
    
    // Update explosions for a few frames
    for (int i = 0; i < 5; i++) {
        missileSystem->update(state, 0.016f);
    }
    
    // Test passes if no crashes occur during explosion handling
    EXPECT_FALSE(state.missileActive);
}
