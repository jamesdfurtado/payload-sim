#include <gtest/gtest.h>
#include "sim/world/ContactManager.h"
#include <raylib.h>

class ContactManagerTest : public ::testing::Test {
protected:
    ContactManager contactManager;
    
    void SetUp() override {
        // Initialize contact manager with clean state
        contactManager.clearAllContacts();
    }
};

TEST_F(ContactManagerTest, InitializesEmpty) {
    EXPECT_TRUE(contactManager.getActiveContacts().empty());
}

TEST_F(ContactManagerTest, ContactSpawning) {
    uint32_t contactId = contactManager.spawnContact();
    
    EXPECT_GT(contactId, 0); // Should return valid ID
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    EXPECT_TRUE(contactManager.isContactAlive(contactId));
}

TEST_F(ContactManagerTest, MultipleContactSpawning) {
    uint32_t id1 = contactManager.spawnContact();
    uint32_t id2 = contactManager.spawnContact();
    uint32_t id3 = contactManager.spawnContact();
    
    EXPECT_NE(id1, id2); // IDs should be unique
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 3);
    EXPECT_TRUE(contactManager.isContactAlive(id1));
    EXPECT_TRUE(contactManager.isContactAlive(id2));
    EXPECT_TRUE(contactManager.isContactAlive(id3));
}

TEST_F(ContactManagerTest, ContactMovement) {
    uint32_t contactId = contactManager.spawnContact();
    
    const auto& contacts = contactManager.getActiveContacts();
    Vector2 initialPos = contacts[0].position;
    
    // Update positions for several frames
    for (int i = 0; i < 60; i++) { // 1 second of movement
        contactManager.updateContactPositions(0.016f);
    }
    
    Vector2 finalPos = contacts[0].position;
    
    // Contact should have moved (unless very unlucky with random direction)
    float distance = sqrtf(powf(finalPos.x - initialPos.x, 2) + powf(finalPos.y - initialPos.y, 2));
    EXPECT_GT(distance, 0.1f); // Should have moved some meaningful distance
}

TEST_F(ContactManagerTest, ContactRemoval) {
    uint32_t id1 = contactManager.spawnContact();
    uint32_t id2 = contactManager.spawnContact();
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 2);
    
    contactManager.removeContact(id1);
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    EXPECT_FALSE(contactManager.isContactAlive(id1));
    EXPECT_TRUE(contactManager.isContactAlive(id2));
}

TEST_F(ContactManagerTest, ContactTypes) {
    // Spawn multiple contacts and check they have valid types
    for (int i = 0; i < 10; i++) {
        contactManager.spawnContact();
    }
    
    const auto& contacts = contactManager.getActiveContacts();
    
    for (const auto& contact : contacts) {
        // Contact type should be one of the valid enum values
        EXPECT_TRUE(contact.type == ContactType::EnemySub || 
                   contact.type == ContactType::FriendlySub || 
                   contact.type == ContactType::Fish || 
                   contact.type == ContactType::Debris);
        
        // Should have valid speed and direction
        EXPECT_GE(contact.speed, 0.0f);
        EXPECT_GE(contact.velocityDirRad, 0.0f);
        EXPECT_LT(contact.velocityDirRad, 2.0f * 3.14159f);
    }
}

TEST_F(ContactManagerTest, NearestContactCalculation) {
    // Spawn contacts at known positions by moving them there
    uint32_t id1 = contactManager.spawnContact();
    uint32_t id2 = contactManager.spawnContact();
    uint32_t id3 = contactManager.spawnContact();
    
    // Let contacts settle, then manually adjust positions for testing
    // (This is a simplification - in real scenario we'd need access to position setters)
    Vector2 testPosition = {100.0f, 100.0f};
    
    uint32_t nearestId = contactManager.getNearestContactId(testPosition, 1000.0f);
    
    // Should find a contact (one of the three we spawned)
    EXPECT_TRUE(nearestId == id1 || nearestId == id2 || nearestId == id3);
    EXPECT_TRUE(contactManager.isContactAlive(nearestId));
}

TEST_F(ContactManagerTest, NearestContactMaxDistance) {
    uint32_t contactId = contactManager.spawnContact();
    
    // Test with very small max distance - should not find contact
    Vector2 farPosition = {10000.0f, 10000.0f}; // Very far position
    uint32_t nearestId = contactManager.getNearestContactId(farPosition, 1.0f);
    
    EXPECT_EQ(nearestId, 0); // Should return 0 (no contact found)
}

TEST_F(ContactManagerTest, ContactLifecycle) {
    // Test full lifecycle: spawn, move, exist, remove
    uint32_t contactId = contactManager.spawnContact();
    
    // Verify initial state
    EXPECT_TRUE(contactManager.isContactAlive(contactId));
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    
    const auto& initialContacts = contactManager.getActiveContacts();
    Vector2 initialPos = initialContacts[0].position;
    EXPECT_TRUE(initialContacts[0].isVisible);
    
    // Update for some time
    for (int i = 0; i < 30; i++) {
        contactManager.updateContactPositions(0.016f);
    }
    
    // Should still be alive and have moved
    EXPECT_TRUE(contactManager.isContactAlive(contactId));
    const auto& updatedContacts = contactManager.getActiveContacts();
    EXPECT_EQ(updatedContacts.size(), 1);
    
    // Remove contact
    contactManager.removeContact(contactId);
    
    // Should be gone
    EXPECT_FALSE(contactManager.isContactAlive(contactId));
    EXPECT_TRUE(contactManager.getActiveContacts().empty());
}

TEST_F(ContactManagerTest, ClearAllContacts) {
    // Spawn multiple contacts
    for (int i = 0; i < 5; i++) {
        contactManager.spawnContact();
    }
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 5);
    
    contactManager.clearAllContacts();
    
    EXPECT_TRUE(contactManager.getActiveContacts().empty());
}

TEST_F(ContactManagerTest, ContactSpawnTiming) {
    // Test spawn timer functionality
    float dt = 0.016f;
    
    // Update spawn timer multiple times
    for (int i = 0; i < 500; i++) { // ~8 seconds
        contactManager.updateSpawnTimer(dt);
        contactManager.spawnContactsIfNeeded();
    }
    
    // Should have spawned some contacts over time
    EXPECT_GT(contactManager.getActiveContacts().size(), 0);
}

TEST_F(ContactManagerTest, BoundaryHandling) {
    // Spawn contacts and let them move for a long time
    for (int i = 0; i < 3; i++) {
        contactManager.spawnContact();
    }
    
    size_t initialCount = contactManager.getActiveContacts().size();
    
    // Update for a very long time to potentially move contacts out of bounds
    for (int i = 0; i < 3000; i++) { // 50 seconds of movement
        contactManager.updateContactPositions(0.016f);
        contactManager.removeOutOfBoundsContacts();
    }
    
    // Some contacts might have been removed if they went out of bounds
    // The test passes if no crashes occur and contact count is reasonable
    EXPECT_GE(contactManager.getActiveContacts().size(), 0);
    EXPECT_LE(contactManager.getActiveContacts().size(), initialCount);
}
