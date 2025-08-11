#include <gtest/gtest.h>
#include "systems/ContactManager.h"
#include <raylib.h>

// Utilities for deterministic setup
static void set_contact(ContactManager& mgr, uint32_t id, Vector2 pos, ContactType type, float speed = 0.0f, float dirRad = 0.0f) {
    SonarContact* c = mgr.findContactById(id);
    ASSERT_NE(c, nullptr);
    c->position = pos;
    c->type = type;
    c->speed = speed;
    c->velocityDirRad = dirRad;
}

class EnemyInteractionTest : public ::testing::Test {
protected:
    void SetUp() override {
        cm = std::make_unique<ContactManager>();
    }

    std::unique_ptr<ContactManager> cm;
};

TEST_F(EnemyInteractionTest, TrackingEnemies_FindInRadiusAndNearest) {
    // Spawn three contacts and set deterministic properties
    uint32_t e1 = cm->spawnContact();
    uint32_t f1 = cm->spawnContact();
    uint32_t e2 = cm->spawnContact();

    set_contact(*cm, e1, {10.0f, 0.0f}, ContactType::EnemySub);
    set_contact(*cm, f1, {12.0f, 0.0f}, ContactType::FriendlySub);
    set_contact(*cm, e2, {30.0f, 0.0f}, ContactType::EnemySub);

    // Find only enemies in radius 20 around origin
    auto enemies = cm->findContactsInRadius({0.0f, 0.0f}, 20.0f, ContactType::EnemySub);
    ASSERT_EQ(enemies.size(), 1u);
    EXPECT_EQ(enemies[0], e1);

    // Nearest to origin should be e1
    EXPECT_EQ(cm->getNearestContactId({0.0f, 0.0f}, 100.0f), e1);

    // When no contact within max distance, return 0
    EXPECT_EQ(cm->getNearestContactId({0.0f, 0.0f}, 5.0f), 0u);
}

TEST_F(EnemyInteractionTest, MissileLaunch_InvalidTarget_DoesNotActivate) {
    cm->launchMissile(9999u, {0.0f, 0.0f});
    EXPECT_FALSE(cm->isMissileActive());
}

TEST_F(EnemyInteractionTest, MissileLaunch_ValidTarget_ActivatesAndEventuallyDeactivates) {
    uint32_t target = cm->spawnContact();
    set_contact(*cm, target, {8.0f, 0.0f}, ContactType::EnemySub);

    cm->launchMissile(target, {0.0f, 0.0f});
    ASSERT_TRUE(cm->isMissileActive());

    // Step until missile finishes (explosion completes)
    // Use conservative cap to avoid infinite loops in case of regressions
    for (int i = 0; i < 200 && cm->isMissileActive(); ++i) {
        cm->updateMissile(0.05f);
    }
    EXPECT_FALSE(cm->isMissileActive());
}

TEST_F(EnemyInteractionTest, MissileHit_RemovesEnemyOnExplosionComplete) {
    uint32_t enemy = cm->spawnContact();
    set_contact(*cm, enemy, {6.0f, 0.0f}, ContactType::EnemySub);

    cm->launchMissile(enemy, {0.0f, 0.0f});
    ASSERT_TRUE(cm->isMissileActive());

    // Run missile until inactive
    int steps = 0;
    while (cm->isMissileActive() && steps < 200) {
        cm->updateMissile(0.05f);
        ++steps;
    }

    EXPECT_FALSE(cm->hasContact(enemy));
    EXPECT_FALSE(cm->isMissileActive());
}

TEST_F(EnemyInteractionTest, MissileTargetDestroyedMidFlight_ExplodesAndClearsState) {
    uint32_t enemy = cm->spawnContact();
    // Place enemy far enough to ensure a flight phase first
    set_contact(*cm, enemy, {120.0f, 0.0f}, ContactType::EnemySub);

    cm->launchMissile(enemy, {0.0f, 0.0f});
    ASSERT_TRUE(cm->isMissileActive());

    // Begin flight
    cm->updateMissile(0.05f);

    // Destroy target before collision
    cm->removeContact(enemy);
    ASSERT_FALSE(cm->hasContact(enemy));

    // Next update should mark target invalid and enter explosion phase
    cm->updateMissile(0.05f);
    const MissileState& ms = cm->getMissileState();
    EXPECT_TRUE(ms.active);
    EXPECT_FALSE(ms.targetValid);

    // Finish explosion
    for (int i = 0; i < 50 && cm->isMissileActive(); ++i) {
        cm->updateMissile(0.05f);
    }
    EXPECT_FALSE(cm->isMissileActive());
}

TEST_F(EnemyInteractionTest, MissileTracksMovingEnemy) {
    uint32_t enemy = cm->spawnContact();
    // Enemy moves along +X slowly; missile should still catch up
    set_contact(*cm, enemy, {60.0f, 0.0f}, ContactType::EnemySub, /*speed=*/15.0f, /*dir=*/0.0f);

    cm->launchMissile(enemy, {0.0f, 0.0f});
    ASSERT_TRUE(cm->isMissileActive());

    // Simulate world: move contact and update missile together
    int steps = 0;
    while (cm->isMissileActive() && steps < 400) {
        cm->updateContactPositions(0.025f);
        cm->updateMissile(0.025f);
        ++steps;
    }

    EXPECT_FALSE(cm->isMissileActive());
    EXPECT_FALSE(cm->hasContact(enemy));
}


