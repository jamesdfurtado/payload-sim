#include <gtest/gtest.h>
#include "sim/systems/TargetValidationSystem.h"
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"

class TargetValidationSystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    std::unique_ptr<TargetValidationSystem> targetValidationSystem;
    SimulationState state;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        targetValidationSystem = std::make_unique<TargetValidationSystem>(*crosshairManager, *contactManager);
        
        state = {};
        state.targetAcquired = false;
        state.targetValidated = false;
    }
    
    uint32_t addEnemySubContact() {
        return contactManager->addContact({100.0f, 100.0f}, ContactType::EnemySub, "EnemySub");
    }
    
    uint32_t addEnemySurfaceContact() {
        return contactManager->addContact({100.0f, 100.0f}, ContactType::ENEMY_SURFACE, "EnemySurface");
    }
};

TEST_F(TargetValidationSystemTest, InitiallyNotValidated) {
    targetValidationSystem->update(state, 0.016f);
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, ValidatesEnemySubTarget) {
    uint32_t subContactId = addEnemySubContact();
    crosshairManager->setTrackedContactId(subContactId);
    state.targetAcquired = true;
    
    targetValidationSystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, DoesNotValidateEnemySurfaceTarget) {
    uint32_t surfaceContactId = addEnemySurfaceContact();
    crosshairManager->setTrackedContactId(surfaceContactId);
    state.targetAcquired = true;
    
    targetValidationSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, InvalidatesWhenTargetNotAcquired) {
    uint32_t subContactId = addEnemySubContact();
    crosshairManager->setTrackedContactId(subContactId);
    state.targetAcquired = true;
    targetValidationSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetValidated);
    
    state.targetAcquired = false;
    targetValidationSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, InvalidatesWhenContactDies) {
    uint32_t subContactId = addEnemySubContact();
    crosshairManager->setTrackedContactId(subContactId);
    state.targetAcquired = true;
    targetValidationSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetValidated);
    
    contactManager->removeContact(subContactId);
    targetValidationSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, InvalidatesWhenContactNotFound) {
    state.targetAcquired = true;
    state.targetValidated = true;
    crosshairManager->setTrackedContactId(999);
    
    targetValidationSystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, HandlesTargetTypeChanges) {
    uint32_t subContactId = addEnemySubContact();
    crosshairManager->setTrackedContactId(subContactId);
    state.targetAcquired = true;
    
    targetValidationSystem->update(state, 0.016f);
    EXPECT_TRUE(state.targetValidated);
    
    uint32_t surfaceContactId = addEnemySurfaceContact();
    crosshairManager->setTrackedContactId(surfaceContactId);
    
    targetValidationSystem->update(state, 0.016f);
    EXPECT_FALSE(state.targetValidated);
}

TEST_F(TargetValidationSystemTest, MaintainsValidationForValidTarget) {
    uint32_t subContactId = addEnemySubContact();
    crosshairManager->setTrackedContactId(subContactId);
    state.targetAcquired = true;
    
    for (int i = 0; i < 10; i++) {
        targetValidationSystem->update(state, 0.016f);
        EXPECT_TRUE(state.targetValidated);
    }
}
