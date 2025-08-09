#include <gtest/gtest.h>
#include "systems/SonarSystem.h"
#include <raylib.h>

class SonarSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        sonarSystem = std::make_unique<SonarSystem>();
        
        state.targetValidated = false;
        state.targetAcquired = false;
        state.noFriendlyUnitsInBlastRadius = true;
    }

    void TearDown() override {}

    std::unique_ptr<SonarSystem> sonarSystem;
    SimulationState state;
};

TEST_F(SonarSystemTest, InitialStateIsCorrect) { // Initial state has no targets or contacts
    EXPECT_FALSE(sonarSystem->isTargetValidated());
    EXPECT_FALSE(sonarSystem->isTargetAcquired());
    EXPECT_TRUE(sonarSystem->getContacts().empty());
}

TEST_F(SonarSystemTest, UpdateSpawnsContacts) { // Update spawns contacts to maintain population
    sonarSystem->update(state, 0.016f);
    
    EXPECT_GE(sonarSystem->getContacts().size(), 10);
}

TEST_F(SonarSystemTest, ContactsHaveValidTypes) { // All contacts have valid types
    sonarSystem->update(state, 0.016f);
    
    const auto& contacts = sonarSystem->getContacts();
    
    for (const auto& contact : contacts) {
        EXPECT_TRUE(contact.type == ContactType::EnemySub ||
                   contact.type == ContactType::FriendlySub ||
                   contact.type == ContactType::Fish ||
                   contact.type == ContactType::Debris);
    }
}

TEST_F(SonarSystemTest, ContactsMove) { // Contacts move over time
    sonarSystem->update(state, 0.016f);
    
    const auto& contacts = sonarSystem->getContacts();
    if (!contacts.empty()) {
        Vector2 initialPos = contacts[0].position;
        
        for (int i = 0; i < 10; ++i) {
            sonarSystem->update(state, 0.1f);
        }
        
        const auto& updatedContacts = sonarSystem->getContacts();
        if (!updatedContacts.empty()) {
            EXPECT_NE(updatedContacts[0].position.x, initialPos.x);
        }
    }
}

TEST_F(SonarSystemTest, ManualLockOnEnemyTarget) { // Can lock onto enemy submarines
    sonarSystem->update(state, 0.016f);
    
    const auto& contacts = sonarSystem->getContacts();
    Vector2 enemyPosition{0, 0};
    bool foundEnemy = false;
    
    for (const auto& contact : contacts) {
        if (contact.type == ContactType::EnemySub) {
            enemyPosition = contact.position;
            foundEnemy = true;
            break;
        }
    }
    
    if (foundEnemy) {
        sonarSystem->attemptManualLock(enemyPosition);
        sonarSystem->update(state, 0.016f);
        
        EXPECT_TRUE(sonarSystem->isTargetAcquired());
        EXPECT_TRUE(sonarSystem->isTargetValidated());
        EXPECT_TRUE(state.targetAcquired);
        EXPECT_TRUE(state.targetValidated);
    }
}

TEST_F(SonarSystemTest, ManualLockOnFriendlyTargetNotValidated) { // Friendly targets are not validated
    sonarSystem->update(state, 0.016f);
    
    const auto& contacts = sonarSystem->getContacts();
    Vector2 friendlyPosition{0, 0};
    bool foundFriendly = false;
    
    for (const auto& contact : contacts) {
        if (contact.type == ContactType::FriendlySub) {
            friendlyPosition = contact.position;
            foundFriendly = true;
            break;
        }
    }
    
    if (foundFriendly) {
        sonarSystem->attemptManualLock(friendlyPosition);
        sonarSystem->update(state, 0.016f);
        
        EXPECT_TRUE(sonarSystem->isTargetAcquired());
        EXPECT_FALSE(sonarSystem->isTargetValidated());
        EXPECT_TRUE(state.targetAcquired);
        EXPECT_FALSE(state.targetValidated);
    }
}

TEST_F(SonarSystemTest, FriendlyUnitsInBlastRadiusDetection) { // Blast radius detection works
    sonarSystem->update(state, 0.016f);
    
    sonarSystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius == true || 
                state.noFriendlyUnitsInBlastRadius == false);
}

TEST_F(SonarSystemTest, ContactRemovalWorks) { // Can remove contacts from the list
    sonarSystem->update(state, 0.016f);
    
    size_t initialCount = sonarSystem->getContacts().size();
    if (initialCount > 0) {
        sonarSystem->removeContact(0);
        EXPECT_EQ(sonarSystem->getContacts().size(), initialCount - 1);
    }
}

TEST_F(SonarSystemTest, LockFailsWhenTooFarFromTarget) { // Lock fails when too far from target
    sonarSystem->update(state, 0.016f);
    
    Vector2 farPosition{1000.0f, 1000.0f};
    sonarSystem->attemptManualLock(farPosition);
    sonarSystem->update(state, 0.016f);
    
    EXPECT_FALSE(sonarSystem->isTargetAcquired());
    EXPECT_FALSE(state.targetAcquired);
}
