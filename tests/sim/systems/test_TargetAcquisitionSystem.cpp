#include <gtest/gtest.h>
#include "sim/systems/TargetAcquisitionSystem.h"
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"

class TargetAcquisitionSystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    std::unique_ptr<TargetAcquisitionSystem> targetAcquisitionSystem;
    SimulationState state;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        targetAcquisitionSystem = std::make_unique<TargetAcquisitionSystem>(*crosshairManager, *contactManager);
        
        state = {};
        state.targetAcquired = false;
    }
    
    uint32_t addTestContact() {
        return contactManager->addContact({100.0f, 100.0f}, ContactType::ENEMY_SURFACE, "TestTarget");
    }
};

TEST_F(TargetAcquisitionSystemTest, InitiallyNoTargetAcquired) {
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, AcquiresTargetWhenCrosshairStartsTracking) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    
    targetAcquisitionSystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, LosesTargetWhenCrosshairStopsTracking) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetAcquired);
    
    crosshairManager->setTrackedContactId(0);
    targetAcquisitionSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, LosesTargetWhenTrackedContactDies) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetAcquired);
    
    contactManager->removeContact(contactId);
    targetAcquisitionSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, MaintainsTargetAcquisitionWhileTracking) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    
    for (int i = 0; i < 10; i++) {
        targetAcquisitionSystem->update(state, 0.016f);
        EXPECT_TRUE(state.targetAcquired);
    }
}

TEST_F(TargetAcquisitionSystemTest, DoesNotAcquireDeadTarget) {
    uint32_t contactId = addTestContact();
    contactManager->removeContact(contactId);
    crosshairManager->setTrackedContactId(contactId);
    
    targetAcquisitionSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetAcquired);
}

TEST_F(TargetAcquisitionSystemTest, HandlesTrackingStateChanges) {
    uint32_t contact1 = addTestContact();
    uint32_t contact2 = addTestContact();
    
    crosshairManager->setTrackedContactId(contact1);
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetAcquired);
    
    crosshairManager->setTrackedContactId(contact2);
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetAcquired);
    
    crosshairManager->setTrackedContactId(0);
    targetAcquisitionSystem->update(state, 0.016f);
    EXPECT_FALSE(state.targetAcquired);
}
