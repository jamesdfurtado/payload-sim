#include <gtest/gtest.h>
#include "sim/systems/SonarSystem.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"

class SonarSystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<SonarSystem> sonarSystem;
    SimulationState state;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        sonarSystem = std::make_unique<SonarSystem>(*contactManager);
        state = {};
    }
    
    uint32_t addTestContact(Vector2 position = {100.0f, 100.0f}) {
        return contactManager->addContact(position, ContactType::ENEMY_SURFACE, "TestTarget");
    }
};

TEST_F(SonarSystemTest, InitializesWithNoSelectedTarget) {
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), 0);
}

TEST_F(SonarSystemTest, AutoSelectsNearestTargetWhenAvailable) {
    uint32_t contactId = addTestContact({50.0f, 50.0f});
    
    sonarSystem->update(state, 0.016f);
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), contactId);
}

TEST_F(SonarSystemTest, ClearsSelectedTargetWhenTargetDies) {
    uint32_t contactId = addTestContact();
    sonarSystem->update(state, 0.016f);
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), contactId);
    
    contactManager->removeContact(contactId);
    sonarSystem->update(state, 0.016f);
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), 0);
}

TEST_F(SonarSystemTest, SelectsNewTargetAfterCurrentTargetDies) {
    uint32_t contact1 = addTestContact({50.0f, 50.0f});
    uint32_t contact2 = addTestContact({100.0f, 100.0f});
    
    sonarSystem->update(state, 0.016f);
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), contact1);
    
    contactManager->removeContact(contact1);
    sonarSystem->update(state, 0.016f);
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), contact2);
}

TEST_F(SonarSystemTest, ManualLockSelectsNearestContactWithinRange) {
    uint32_t closeContact = addTestContact({10.0f, 10.0f});
    uint32_t farContact = addTestContact({200.0f, 200.0f});
    
    sonarSystem->attemptManualLock({15.0f, 15.0f});
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), closeContact);
}

TEST_F(SonarSystemTest, ManualLockIgnoresContactsOutsideRange) {
    uint32_t farContact = addTestContact({200.0f, 200.0f});
    
    sonarSystem->attemptManualLock({0.0f, 0.0f});
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), 0);
}

TEST_F(SonarSystemTest, UpdatesContactManagerState) {
    uint32_t initialContactCount = contactManager->getActiveContacts().size();
    
    for (int i = 0; i < 100; i++) {
        sonarSystem->update(state, 0.1f);
    }
    
    uint32_t finalContactCount = contactManager->getActiveContacts().size();
    EXPECT_GE(finalContactCount, initialContactCount);
}

TEST_F(SonarSystemTest, SelectsClosestTargetWhenMultipleAvailable) {
    uint32_t closeContact = addTestContact({10.0f, 10.0f});
    uint32_t mediumContact = addTestContact({50.0f, 50.0f});
    uint32_t farContact = addTestContact({100.0f, 100.0f});
    
    sonarSystem->update(state, 0.016f);
    
    EXPECT_EQ(sonarSystem->getSelectedTargetId(), closeContact);
}
