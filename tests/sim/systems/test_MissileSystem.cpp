#include <gtest/gtest.h>
#include "sim/systems/MissileSystem.h"
#include "sim/world/MissileManager.h"
#include "sim/world/ContactManager.h"
#include "sim/world/CrosshairManager.h"
#include "sim/SimulationState.h"

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
        
        state = {};
        state.targetAcquired = true;
        state.missileLaunched = false;
        state.missileActive = false;
        state.explosionActive = false;
        state.missileTargetId = 0;
    }
    
    uint32_t addTestContact() {
        return contactManager->addContact({100.0f, 100.0f}, ContactType::ENEMY_SURFACE, "TestTarget");
    }
};

TEST_F(MissileSystemTest, TriggersLaunchWhenConditionsMet) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, contactId);
    EXPECT_FALSE(state.missileLaunched);
}

TEST_F(MissileSystemTest, DoesNotLaunchWithoutTarget) {
    state.missileLaunched = true;
    state.targetAcquired = false;
    
    missileSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, 0);
}

TEST_F(MissileSystemTest, DoesNotLaunchWithoutTrackedContact) {
    state.missileLaunched = true;
    crosshairManager->setTrackedContactId(0);
    
    missileSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, 0);
}

TEST_F(MissileSystemTest, ExplodesWhenTargetLost) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    EXPECT_TRUE(state.missileActive);
    
    crosshairManager->setTrackedContactId(0);
    missileSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, 0);
}

TEST_F(MissileSystemTest, ExplodesWhenTrackedContactDies) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    EXPECT_TRUE(state.missileActive);
    
    contactManager->removeContact(contactId);
    missileSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, 0);
}

TEST_F(MissileSystemTest, UpdatesGuidanceForValidTarget) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    EXPECT_TRUE(state.missileActive);
    
    missileSystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.missileActive);
    EXPECT_EQ(state.missileTargetId, contactId);
}

TEST_F(MissileSystemTest, HandlesMissileContactCollisions) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    
    for (int i = 0; i < 200; i++) {
        missileSystem->update(state, 0.016f);
        if (!contactManager->isContactAlive(contactId)) {
            break;
        }
    }
    
    EXPECT_FALSE(contactManager->isContactAlive(contactId));
}

TEST_F(MissileSystemTest, UpdatesExplosionState) {
    uint32_t contactId = addTestContact();
    crosshairManager->setTrackedContactId(contactId);
    state.missileLaunched = true;
    
    missileSystem->update(state, 0.016f);
    crosshairManager->setTrackedContactId(0);
    missileSystem->update(state, 0.016f);
    
    bool hadExplosion = false;
    for (int i = 0; i < 100; i++) {
        missileSystem->update(state, 0.016f);
        if (state.explosionActive) {
            hadExplosion = true;
        }
    }
    
    EXPECT_TRUE(hadExplosion);
}
