#include <gtest/gtest.h>
#include "sim/world/ContactManager.h"

class ContactManagerTest : public ::testing::Test {
protected:
    ContactManager contactManager;
    
    void SetUp() override {
    }
};

TEST_F(ContactManagerTest, InitializesEmpty) {
    EXPECT_EQ(contactManager.getActiveContacts().size(), 0);
}

TEST_F(ContactManagerTest, SpawnsContactWithValidId) {
    uint32_t contactId = contactManager.spawnContact();
    
    EXPECT_GT(contactId, 0);
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    EXPECT_TRUE(contactManager.isContactAlive(contactId));
}

TEST_F(ContactManagerTest, SpawnsContactsWithUniqueIds) {
    uint32_t id1 = contactManager.spawnContact();
    uint32_t id2 = contactManager.spawnContact();
    uint32_t id3 = contactManager.spawnContact();
    
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}

TEST_F(ContactManagerTest, RemovesContactById) {
    uint32_t contactId = contactManager.spawnContact();
    EXPECT_TRUE(contactManager.isContactAlive(contactId));
    
    contactManager.removeContact(contactId);
    
    EXPECT_FALSE(contactManager.isContactAlive(contactId));
    EXPECT_EQ(contactManager.getActiveContacts().size(), 0);
}

TEST_F(ContactManagerTest, ClearsAllContacts) {
    contactManager.spawnContact();
    contactManager.spawnContact();
    contactManager.spawnContact();
    EXPECT_EQ(contactManager.getActiveContacts().size(), 3);
    
    contactManager.clearAllContacts();
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 0);
}

TEST_F(ContactManagerTest, FindsNearestContact) {
    uint32_t closeContact = contactManager.addContact({10.0f, 10.0f}, ContactType::EnemySub, "Close");
    uint32_t farContact = contactManager.addContact({100.0f, 100.0f}, ContactType::EnemySub, "Far");
    
    uint32_t nearestId = contactManager.getNearestContactId({5.0f, 5.0f});
    
    EXPECT_EQ(nearestId, closeContact);
}

TEST_F(ContactManagerTest, ReturnsZeroWhenNoContactsInRange) {
    contactManager.addContact({200.0f, 200.0f}, ContactType::EnemySub, "Far");
    
    uint32_t nearestId = contactManager.getNearestContactId({0.0f, 0.0f}, 10.0f);
    
    EXPECT_EQ(nearestId, 0);
}

TEST_F(ContactManagerTest, UpdatesContactPositions) {
    uint32_t contactId = contactManager.spawnContact();
    const auto& contacts = contactManager.getActiveContacts();
    Vector2 initialPos = contacts[0].position;
    
    contactManager.updateContactPositions(1.0f);
    
    Vector2 newPos = contactManager.getActiveContacts()[0].position;
    EXPECT_NE(initialPos.x, newPos.x);
    EXPECT_NE(initialPos.y, newPos.y);
}

TEST_F(ContactManagerTest, EnsuresEnemySubPresence) {
    contactManager.spawnContactsIfNeeded();
    
    bool hasEnemySub = false;
    for (const auto& contact : contactManager.getActiveContacts()) {
        if (contact.type == ContactType::EnemySub) {
            hasEnemySub = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasEnemySub);
}

TEST_F(ContactManagerTest, SpawnsContactsToMaintainMinimum) {
    contactManager.spawnContactsIfNeeded();
    
    EXPECT_GE(contactManager.getActiveContacts().size(), 10);
}

TEST_F(ContactManagerTest, RemovesOutOfBoundsContacts) {
    contactManager.addContact({-700.0f, 0.0f}, ContactType::EnemySub, "OutOfBounds");
    contactManager.addContact({0.0f, 0.0f}, ContactType::EnemySub, "InBounds");
    
    contactManager.removeOutOfBoundsContacts();
    
    EXPECT_EQ(contactManager.getActiveContacts().size(), 1);
    EXPECT_EQ(contactManager.getActiveContacts()[0].position.x, 0.0f);
}

TEST_F(ContactManagerTest, UpdatesSpawnTimer) {
    float initialTimer = 1.5f;
    contactManager.updateSpawnTimer(0.5f);
    contactManager.updateSpawnTimer(0.5f);
    contactManager.updateSpawnTimer(0.6f);
    
    contactManager.spawnContactsIfNeeded();
    
    size_t contactCount = contactManager.getActiveContacts().size();
    EXPECT_GT(contactCount, 10);
}
