#include <gtest/gtest.h>
#include "sim/systems/FriendlySafetySystem.h"
#include "sim/SimulationState.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"
#include <algorithm>

class FriendlySafetySystemTest : public ::testing::Test {
protected:
    ContactManager contactManager;
    CrosshairManager crosshairManager{contactManager};
    FriendlySafetySystem safetySystem{crosshairManager, contactManager};
    SimulationState state;
    
    void SetUp() override {
        state.noFriendlyUnitsInBlastRadius = true;
        contactManager.clearAllContacts();
    }
};

TEST_F(FriendlySafetySystemTest, InitializesCorrectly) {
    EXPECT_STREQ(safetySystem.getName(), "FriendlySafetySystem");
}

TEST_F(FriendlySafetySystemTest, DefaultSafeState) {
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, FriendlyDetection) {
    // Test detection of friendly units in blast radius
    
    // Initially safe - no target acquired
    safetySystem.update(state, 0.016f);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    
    // Spawn a friendly submarine
    uint32_t friendlyId = 0;
    for (int i = 0; i < 20; i++) { // Try multiple times to get a friendly
        uint32_t id = contactManager.spawnContact();
        const auto& contacts = contactManager.getActiveContacts();
        auto it = std::find_if(contacts.begin(), contacts.end(), 
            [id](const SonarContact& c) { return c.id == id; });
        
        if (it != contacts.end() && it->type == ContactType::FriendlySub) {
            friendlyId = id;
            break;
        } else {
            contactManager.removeContact(id); // Remove non-friendly contacts
        }
    }
    
    if (friendlyId > 0) {
        // Set up target tracking near the friendly
        const auto& contacts = contactManager.getActiveContacts();
        Vector2 friendlyPos = contacts[0].position;
        
        // Set up tracking through simulated mouse click at friendly position
        Rectangle sonarBounds = {10, 10, 400, 300};
        float nx = (friendlyPos.x + 600.0f) / 1200.0f;
        float ny = (friendlyPos.y + 360.0f) / 720.0f;
        Vector2 friendlyScreenPos = {
            sonarBounds.x + nx * sonarBounds.width,
            sonarBounds.y + ny * sonarBounds.height
        };
        
        crosshairManager.updateMousePosition(friendlyScreenPos, sonarBounds);
        crosshairManager.handleMouseClick(friendlyScreenPos, sonarBounds);
        state.targetAcquired = true;
        
        safetySystem.update(state, 0.016f);
        
        // Should detect friendly in blast radius and mark as unsafe
        EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
    }
}

TEST_F(FriendlySafetySystemTest, SafetyValidation) {
    // Test safety validation with no friendlies
    
    // Spawn enemy contact only
    uint32_t enemyId = 0;
    for (int i = 0; i < 20; i++) { // Try multiple times to get an enemy
        uint32_t id = contactManager.spawnContact();
        const auto& contacts = contactManager.getActiveContacts();
        auto it = std::find_if(contacts.begin(), contacts.end(), 
            [id](const SonarContact& c) { return c.id == id; });
        
        if (it != contacts.end() && it->type == ContactType::EnemySub) {
            enemyId = id;
            break;
        } else {
            contactManager.removeContact(id); // Remove non-enemy contacts
        }
    }
    
    if (enemyId > 0) {
        // Set up target tracking on enemy (should be safe)
        const auto& contacts = contactManager.getActiveContacts();
        Vector2 enemyPos = contacts[0].position;
        
        // Set up tracking through simulated mouse click at enemy position  
        Rectangle sonarBounds = {10, 10, 400, 300};
        float nx = (enemyPos.x + 600.0f) / 1200.0f;
        float ny = (enemyPos.y + 360.0f) / 720.0f;
        Vector2 enemyScreenPos = {
            sonarBounds.x + nx * sonarBounds.width,
            sonarBounds.y + ny * sonarBounds.height
        };
        
        crosshairManager.updateMousePosition(enemyScreenPos, sonarBounds);
        crosshairManager.handleMouseClick(enemyScreenPos, sonarBounds);
        state.targetAcquired = true;
        
        safetySystem.update(state, 0.016f);
        
        // Should be safe (no friendlies in blast radius)
        EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
    }
}

TEST_F(FriendlySafetySystemTest, BlastRadiusCalculation) {
    // Test blast radius calculation and center position safety
    
    // Test our own submarine at center being in blast radius
    // Set crosshair position at center (0, 0) - where our submarine is
    Rectangle sonarBounds = {10, 10, 400, 300};
    // Center of world (0,0) converts to center of sonar bounds
    Vector2 centerScreenPos = {
        sonarBounds.x + sonarBounds.width * 0.5f,
        sonarBounds.y + sonarBounds.height * 0.5f
    };
    
    crosshairManager.updateMousePosition(centerScreenPos, sonarBounds);
    crosshairManager.handleMouseClick(centerScreenPos, sonarBounds);
    state.targetAcquired = true;
    
    safetySystem.update(state, 0.016f);
    
    // Should be unsafe because our own submarine is at blast center
    EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
    
    // Test targeting far from center (should be safe if no other friendlies) 
    Vector2 farWorldPos = {1000.0f, 1000.0f};
    float nx2 = (farWorldPos.x + 600.0f) / 1200.0f;
    float ny2 = (farWorldPos.y + 360.0f) / 720.0f;
    Vector2 farScreenPos = {
        sonarBounds.x + nx2 * sonarBounds.width,
        sonarBounds.y + ny2 * sonarBounds.height
    };
    
    crosshairManager.updateMousePosition(farScreenPos, sonarBounds);
    crosshairManager.handleMouseClick(farScreenPos, sonarBounds);
    
    safetySystem.update(state, 0.016f);
    
    // Should be safe (blast center far from our submarine)
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}
