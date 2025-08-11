#include <gtest/gtest.h>
#include "systems/ContactManager.h"
#include <raylib.h>

class ContactManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
    }

    void TearDown() override {}

    std::unique_ptr<ContactManager> contactManager;
};

TEST_F(ContactManagerTest, InitialStateIsCorrect) {
    EXPECT_EQ(contactManager->getContactCount(), 0);
    EXPECT_TRUE(contactManager->getActiveContacts().empty());
}

TEST_F(ContactManagerTest, SpawnContactAssignsUniqueId) {
    uint32_t id1 = contactManager->spawnContact();
    uint32_t id2 = contactManager->spawnContact();
    
    EXPECT_EQ(id1, 1);
    EXPECT_EQ(id2, 2);
    EXPECT_EQ(contactManager->getContactCount(), 2);
}

TEST_F(ContactManagerTest, FindContactByIdWorks) {
    uint32_t id = contactManager->spawnContact();
    
    const SonarContact* found = contactManager->findContactById(id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->id, id);
}

TEST_F(ContactManagerTest, RemoveContactByIdWorks) {
    uint32_t id = contactManager->spawnContact();
    EXPECT_EQ(contactManager->getContactCount(), 1);
    
    contactManager->removeContact(id);
    EXPECT_EQ(contactManager->getContactCount(), 0);
    
    const SonarContact* found = contactManager->findContactById(id);
    EXPECT_EQ(found, nullptr);
}

TEST_F(ContactManagerTest, ContactIdsAreStable) {
    uint32_t id1 = contactManager->spawnContact();
    uint32_t id2 = contactManager->spawnContact();
    
    // Remove first contact
    contactManager->removeContact(id1);
    EXPECT_EQ(contactManager->getContactCount(), 1);
    
    // Spawn new contact - should get next available ID
    uint32_t id3 = contactManager->spawnContact();
    EXPECT_EQ(id3, 3);  // Not 1, since 1 was used
    
    // Original contact 2 should still exist
    const SonarContact* found = contactManager->findContactById(id2);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->id, id2);
}

TEST_F(ContactManagerTest, FindContactsInRadiusWorks) {
    // Spawn an enemy contact at a known position
    uint32_t id = contactManager->spawnContact();
    SonarContact* contact = contactManager->findContactById(id);
    if (contact) {
        contact->type = ContactType::EnemySub;
        contact->position = {100.0f, 100.0f};
        
        // Find contacts within radius
        auto foundIds = contactManager->findContactsInRadius({100.0f, 100.0f}, 50.0f, ContactType::EnemySub);
        EXPECT_EQ(foundIds.size(), 1);
        EXPECT_EQ(foundIds[0], id);
    }
}
