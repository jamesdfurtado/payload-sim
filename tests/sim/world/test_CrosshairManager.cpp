#include <gtest/gtest.h>
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"

class CrosshairManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<ContactManager> contactManager;
    std::unique_ptr<CrosshairManager> crosshairManager;
    Rectangle sonarBounds;
    
    void SetUp() override {
        contactManager = std::make_unique<ContactManager>();
        crosshairManager = std::make_unique<CrosshairManager>(*contactManager);
        sonarBounds = {100.0f, 100.0f, 400.0f, 300.0f};
    }
    
    uint32_t addTestContact(Vector2 position = {0.0f, 0.0f}) {
        return contactManager->addContact(position, ContactType::EnemySub, "TestContact");
    }
};

TEST_F(CrosshairManagerTest, InitializesNotTracking) {
    EXPECT_FALSE(crosshairManager->isTracking());
    EXPECT_EQ(crosshairManager->getTrackedContactId(), 0);
}

TEST_F(CrosshairManagerTest, TracksContactWhenClicked) {
    uint32_t contactId = addTestContact({0.0f, 0.0f});
    Vector2 screenPos = {300.0f, 250.0f};
    
    crosshairManager->updateMousePosition(screenPos, sonarBounds);
    bool clicked = crosshairManager->handleMouseClick(screenPos, sonarBounds);
    
    EXPECT_TRUE(clicked);
    EXPECT_TRUE(crosshairManager->isTracking());
    EXPECT_EQ(crosshairManager->getTrackedContactId(), contactId);
}

TEST_F(CrosshairManagerTest, DoesNotTrackWhenClickOutsideSonar) {
    addTestContact({0.0f, 0.0f});
    Vector2 outsidePos = {50.0f, 50.0f};
    
    crosshairManager->updateMousePosition(outsidePos, sonarBounds);
    bool clicked = crosshairManager->handleMouseClick(outsidePos, sonarBounds);
    
    EXPECT_FALSE(clicked);
    EXPECT_FALSE(crosshairManager->isTracking());
}

TEST_F(CrosshairManagerTest, UpdatesCrosshairPositionWhenTracking) {
    uint32_t contactId = addTestContact({50.0f, 50.0f});
    crosshairManager->setTrackedContactId(contactId);
    
    crosshairManager->update(0.016f);
    
    Vector2 crosshairPos = crosshairManager->getCrosshairPosition();
    EXPECT_FLOAT_EQ(crosshairPos.x, 50.0f);
    EXPECT_FLOAT_EQ(crosshairPos.y, 50.0f);
}

TEST_F(CrosshairManagerTest, StopsTrackingWhenContactRemoved) {
    uint32_t contactId = addTestContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(contactId);
    EXPECT_TRUE(crosshairManager->isTracking());
    
    contactManager->removeContact(contactId);
    crosshairManager->update(0.016f);
    
    EXPECT_FALSE(crosshairManager->isTracking());
    EXPECT_EQ(crosshairManager->getTrackedContactId(), 0);
}

TEST_F(CrosshairManagerTest, UpdatesMousePositionAndSonarState) {
    Vector2 insidePos = {300.0f, 250.0f};
    Vector2 outsidePos = {50.0f, 50.0f};
    
    crosshairManager->updateMousePosition(insidePos, sonarBounds);
    EXPECT_TRUE(crosshairManager->isMouseOverSonar());
    EXPECT_EQ(crosshairManager->getMousePosition().x, insidePos.x);
    EXPECT_EQ(crosshairManager->getMousePosition().y, insidePos.y);
    
    crosshairManager->updateMousePosition(outsidePos, sonarBounds);
    EXPECT_FALSE(crosshairManager->isMouseOverSonar());
}

TEST_F(CrosshairManagerTest, RequiresContactWithinSelectionRadius) {
    addTestContact({0.0f, 0.0f});
    addTestContact({50.0f, 50.0f});
    
    Vector2 nearFirstContact = {300.0f, 250.0f};
    Vector2 nearSecondContact = {350.0f, 300.0f};
    
    crosshairManager->updateMousePosition(nearFirstContact, sonarBounds);
    bool clicked1 = crosshairManager->handleMouseClick(nearFirstContact, sonarBounds);
    EXPECT_TRUE(clicked1);
    
    crosshairManager->clearTracking();
    
    crosshairManager->updateMousePosition(nearSecondContact, sonarBounds);
    bool clicked2 = crosshairManager->handleMouseClick(nearSecondContact, sonarBounds);
    EXPECT_TRUE(clicked2);
}

TEST_F(CrosshairManagerTest, ClearsTrackingManually) {
    uint32_t contactId = addTestContact({0.0f, 0.0f});
    crosshairManager->setTrackedContactId(contactId);
    EXPECT_TRUE(crosshairManager->isTracking());
    
    crosshairManager->clearTracking();
    
    EXPECT_FALSE(crosshairManager->isTracking());
    EXPECT_EQ(crosshairManager->getTrackedContactId(), 0);
}

TEST_F(CrosshairManagerTest, HandlesMultipleContactSelection) {
    uint32_t contact1 = addTestContact({-50.0f, 0.0f});
    uint32_t contact2 = addTestContact({50.0f, 0.0f});
    
    Vector2 leftClick = {250.0f, 250.0f};
    Vector2 rightClick = {350.0f, 250.0f};
    
    crosshairManager->updateMousePosition(leftClick, sonarBounds);
    crosshairManager->handleMouseClick(leftClick, sonarBounds);
    EXPECT_EQ(crosshairManager->getTrackedContactId(), contact1);
    
    crosshairManager->updateMousePosition(rightClick, sonarBounds);
    crosshairManager->handleMouseClick(rightClick, sonarBounds);
    EXPECT_EQ(crosshairManager->getTrackedContactId(), contact2);
}
