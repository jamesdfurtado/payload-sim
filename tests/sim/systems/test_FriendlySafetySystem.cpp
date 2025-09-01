#include <gtest/gtest.h>
#include "sim/systems/FriendlySafetySystem.h"
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"
#include "sim/SimulationState.h"

class FriendlySafetySystemTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    std::unique_ptr<FriendlySafetySystem> friendlySafetySystem;
    SimulationState state;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        friendlySafetySystem = std::make_unique<FriendlySafetySystem>(*crosshairManager, *contactManager);
        
        state = {};
        state.targetAcquired = false;
        state.noFriendlyUnitsInBlastRadius = true;
    }
    
    uint32_t addFriendlySubContact(Vector2 position) {
        return contactManager->addContact(position, ContactType::FriendlySub, "FriendlySub");
    }
    
    uint32_t addEnemyContact(Vector2 position) {
        return contactManager->addContact(position, ContactType::ENEMY_SURFACE, "Enemy");
    }
};

TEST_F(FriendlySafetySystemTest, InitiallyNoFriendliesInBlastRadius) {
    friendlySafetySystem->update(state, 0.016f);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, SafeWhenNotTracking) {
    addFriendlySubContact({10.0f, 10.0f});
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, SafeWhenTargetNotAcquired) {
    addFriendlySubContact({10.0f, 10.0f});
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, DetectsFriendlySubInBlastRadius) {
    addFriendlySubContact({10.0f, 10.0f});
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, SafeWhenFriendlyOutsideBlastRadius) {
    addFriendlySubContact({100.0f, 100.0f});
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, DetectsOwnSubmarineInBlastRadius) {
    crosshairManager->setCrosshairPosition({30.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({30.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, IgnoresEnemyContactsInBlastRadius) {
    addEnemyContact({10.0f, 10.0f});
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, HandlesMultipleFriendlyContacts) {
    addFriendlySubContact({100.0f, 100.0f});
    addFriendlySubContact({10.0f, 10.0f});
    addFriendlySubContact({200.0f, 200.0f});
    
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    friendlySafetySystem->update(state, 0.016f);
    
    EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
}

TEST_F(FriendlySafetySystemTest, UpdatesStateWhenCrosshairMoves) {
    addFriendlySubContact({10.0f, 10.0f});
    uint32_t enemyId = addEnemyContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(enemyId);
    state.targetAcquired = true;
    
    crosshairManager->setCrosshairPosition({0.0f, 0.0f});
    friendlySafetySystem->update(state, 0.016f);
    EXPECT_FALSE(state.noFriendlyUnitsInBlastRadius);
    
    crosshairManager->setCrosshairPosition({100.0f, 100.0f});
    friendlySafetySystem->update(state, 0.016f);
    EXPECT_TRUE(state.noFriendlyUnitsInBlastRadius);
}
