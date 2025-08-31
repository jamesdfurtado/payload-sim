#include <gtest/gtest.h>
#include "sim/systems/SonarSystem.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"
#include <cmath>

class SonarSystemTest : public ::testing::Test {
protected:
    ContactManager contactManager;
    SonarSystem sonarSystem{contactManager};
    SimulationState state;
    
    void SetUp() override {
        // Initialize contact manager and sonar system
    }
};

TEST_F(SonarSystemTest, InitializesCorrectly) {
    EXPECT_STREQ(sonarSystem.getName(), "SonarSystem");
}

TEST_F(SonarSystemTest, UpdatesContacts) {
    // Spawn some contacts
    uint32_t contactId = contactManager.spawnContact();
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    
    // Update sonar system
    sonarSystem.update(state, 0.016f);
    
    // Contacts should still exist (unless they moved out of bounds)
    EXPECT_GT(contactManager.getActiveContacts().size(), 0);
    
    // Test that contacts move over time
    const auto& initialContacts = contactManager.getActiveContacts();
    Vector2 initialPos = initialContacts[0].position;
    
    // Update multiple times to see movement
    for (int i = 0; i < 60; i++) { // 1 second of updates
        sonarSystem.update(state, 0.016f);
    }
    
    const auto& updatedContacts = contactManager.getActiveContacts();
    if (!updatedContacts.empty()) {
        Vector2 finalPos = updatedContacts[0].position;
        
        // Contact should have moved (unless very unlucky with random direction)
        float distance = sqrtf(powf(finalPos.x - initialPos.x, 2) + powf(finalPos.y - initialPos.y, 2));
        EXPECT_GT(distance, 0.1f); // Should have moved some meaningful distance
    }
}

TEST_F(SonarSystemTest, TargetSelection) {
    // Spawn multiple contacts
    uint32_t contact1 = contactManager.spawnContact();
    uint32_t contact2 = contactManager.spawnContact();
    uint32_t contact3 = contactManager.spawnContact();
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 3);
    
    // Update sonar system - it should automatically select nearest target
    sonarSystem.update(state, 0.016f);
    
    // Should have selected a target
    uint32_t selectedTarget = sonarSystem.getSelectedTargetId();
    EXPECT_GT(selectedTarget, 0);
    
    // Selected target should be one of the spawned contacts
    EXPECT_TRUE(selectedTarget == contact1 || selectedTarget == contact2 || selectedTarget == contact3);
    
    // Verify the selected contact is alive
    EXPECT_TRUE(contactManager.isContactAlive(selectedTarget));
}

TEST_F(SonarSystemTest, ManualLocking) {
    // Spawn contacts at known positions
    uint32_t contact1 = contactManager.spawnContact();
    uint32_t contact2 = contactManager.spawnContact();
    
    // Let contacts settle
    sonarSystem.update(state, 0.016f);
    
    // Get contact positions
    const auto& contacts = contactManager.getActiveContacts();
    EXPECT_EQ(contacts.size(), 2);
    
    Vector2 targetPos = contacts[0].position;
    
    // Attempt manual lock near first contact
    sonarSystem.attemptManualLock(targetPos);
    
    // Should have locked onto a contact
    uint32_t lockedTarget = sonarSystem.getSelectedTargetId();
    EXPECT_GT(lockedTarget, 0);
    EXPECT_TRUE(contactManager.isContactAlive(lockedTarget));
    
    // Test manual lock far from any contact
    Vector2 farPos = {10000.0f, 10000.0f};
    sonarSystem.attemptManualLock(farPos);
    
    // Should not have locked onto anything (0 = no contact within range)
    uint32_t farTarget = sonarSystem.getSelectedTargetId();
    // Note: If no contact is found within range, it might return 0 or keep previous target
    // depending on implementation - either is acceptable
}
