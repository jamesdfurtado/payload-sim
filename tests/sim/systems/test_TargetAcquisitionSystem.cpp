#include <gtest/gtest.h>
#include "sim/systems/TargetAcquisitionSystem.h"
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"

class TargetAcquisitionSystemTest : public ::testing::Test {
protected:
    CrosshairManager crosshairManager;
    ContactManager contactManager;
    TargetAcquisitionSystem acquisitionSystem{crosshairManager, contactManager};
    SimulationState state;
    
    void SetUp() override {
        state.targetAcquired = false;
    }
};

TEST_F(TargetAcquisitionSystemTest, InitializesCorrectly) {
    EXPECT_STREQ(acquisitionSystem.getName(), "TargetAcquisitionSystem");
}

TEST_F(TargetAcquisitionSystemTest, NoTargetByDefault) {
    acquisitionSystem.update(state, 0.1f);
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, TargetAcquisition) {
    // Spawn enemy contact
    uint32_t enemyId = contactManager.spawnContact();
    
    // Select target with crosshair
    const auto& contacts = contactManager.getActiveContacts();
    if (!contacts.empty()) {
        Vector2 targetPos = contacts[0].position;
        crosshairManager.setTargetingCirclePosition(targetPos);
        crosshairManager.trackContact(enemyId);
    }
    
    // Update target acquisition system
    acquisitionSystem.update(state, 0.016f);
    
    // Should acquire target
    EXPECT_TRUE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, TargetLoss) {
    // Setup: acquire a target first
    uint32_t enemyId = contactManager.spawnContact();
    const auto& contacts = contactManager.getActiveContacts();
    
    if (!contacts.empty()) {
        Vector2 targetPos = contacts[0].position;
        crosshairManager.setTargetingCirclePosition(targetPos);
        crosshairManager.trackContact(enemyId);
        acquisitionSystem.update(state, 0.016f);
        EXPECT_TRUE(state.targetAcquired);
    }
    
    // Remove the contact
    contactManager.removeContact(enemyId);
    acquisitionSystem.update(state, 0.016f);
    
    // Should lose target
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, NoAcquisitionWithoutTracking) {
    // Spawn enemy but don't track with crosshair
    contactManager.spawnContact();
    
    acquisitionSystem.update(state, 0.016f);
    
    // Should not acquire target without crosshair selection
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, TargetPersistence) {
    // Acquire target
    uint32_t enemyId = contactManager.spawnContact();
    const auto& contacts = contactManager.getActiveContacts();
    
    if (!contacts.empty()) {
        Vector2 targetPos = contacts[0].position;
        crosshairManager.setTargetingCirclePosition(targetPos);
        crosshairManager.trackContact(enemyId);
    }
    
    // Update over multiple frames
    for (int i = 0; i < 60; i++) { // 1 second
        contactManager.updateContactPositions(0.016f);
        acquisitionSystem.update(state, 0.016f);
        
        // Target should remain acquired while contact exists
        EXPECT_TRUE(state.targetAcquired);
    }
}

TEST_F(TargetAcquisitionSystemTest, MultipleTargetsHandling) {
    // Spawn multiple enemies
    uint32_t enemy1 = contactManager.spawnContact();
    uint32_t enemy2 = contactManager.spawnContact();
    
    // Track specific enemy with crosshair
    crosshairManager.trackContact(enemy2);
    
    acquisitionSystem.update(state, 0.016f);
    
    // Should acquire the tracked target
    EXPECT_TRUE(state.targetAcquired);
    EXPECT_EQ(crosshairManager.getTrackedContactId(), enemy2);
}
