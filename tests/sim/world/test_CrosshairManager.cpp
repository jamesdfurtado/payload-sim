#include <gtest/gtest.h>
#include "sim/world/CrosshairManager.h"
#include "sim/world/ContactManager.h"

class CrosshairManagerTest : public ::testing::Test {
protected:
    ContactManager contactManager;
    CrosshairManager crosshairManager{contactManager};
    
    void SetUp() override {
        contactManager.clearAllContacts();
    }
};

TEST_F(CrosshairManagerTest, DefaultState) {
    EXPECT_FALSE(crosshairManager.isTracking());
}

TEST_F(CrosshairManagerTest, MouseTracking) {
    // Test mouse position tracking
    Rectangle sonarBounds = {10, 10, 400, 300};
    Vector2 mousePos = {100, 50};
    
    // Initially mouse not over sonar
    EXPECT_FALSE(crosshairManager.isMouseOverSonar());
    
    // Update mouse position over sonar
    crosshairManager.updateMousePosition(mousePos, sonarBounds);
    
    // Should detect mouse over sonar
    EXPECT_TRUE(crosshairManager.isMouseOverSonar());
    EXPECT_FLOAT_EQ(crosshairManager.getMousePosition().x, mousePos.x);
    EXPECT_FLOAT_EQ(crosshairManager.getMousePosition().y, mousePos.y);
    
    // Move mouse outside sonar bounds
    Vector2 outsidePos = {500, 500};
    crosshairManager.updateMousePosition(outsidePos, sonarBounds);
    
    // Should no longer be over sonar
    EXPECT_FALSE(crosshairManager.isMouseOverSonar());
}

TEST_F(CrosshairManagerTest, TargetSelection) {
    // Test target selection via crosshair clicks
    Rectangle sonarBounds = {10, 10, 400, 300};
    
    // Spawn a contact
    uint32_t contactId = contactManager.spawnContact();
    EXPECT_GT(contactId, 0);
    
    // Get contact position and convert to screen coordinates
    const auto& contacts = contactManager.getActiveContacts();
    EXPECT_EQ(contacts.size(), 1);
    
    Vector2 contactWorldPos = contacts[0].position;
    
    // Convert world position to screen position (reverse of screenToWorld calculation)
    // World space: -600..600, -360..360 -> Screen space: bounds
    float nx = (contactWorldPos.x + 600.0f) / 1200.0f;
    float ny = (contactWorldPos.y + 360.0f) / 720.0f;
    Vector2 contactScreenPos = {
        sonarBounds.x + nx * sonarBounds.width,
        sonarBounds.y + ny * sonarBounds.height
    };
    
    // Update mouse position and click on contact
    crosshairManager.updateMousePosition(contactScreenPos, sonarBounds);
    EXPECT_TRUE(crosshairManager.isMouseOverSonar());
    
    bool clicked = crosshairManager.handleMouseClick(contactScreenPos, sonarBounds);
    EXPECT_TRUE(clicked);
    
    // Should now be tracking the contact
    EXPECT_TRUE(crosshairManager.isTracking());
    EXPECT_EQ(crosshairManager.getTrackedContactId(), contactId);
}

TEST_F(CrosshairManagerTest, TrackingState) {
    // Test tracking state management
    
    // Initially not tracking
    EXPECT_FALSE(crosshairManager.isTracking());
    EXPECT_EQ(crosshairManager.getTrackedContactId(), 0);
    
    // Spawn and track a contact
    uint32_t contactId = contactManager.spawnContact();
    const auto& contacts = contactManager.getActiveContacts();
    Vector2 contactPos = contacts[0].position;
    
    // Set up tracking through simulated mouse click at contact position
    Vector2 contactWorldPos = contacts[0].position;
    Rectangle sonarBounds = {10, 10, 400, 300};
    
    // Convert world position to screen position
    float nx = (contactWorldPos.x + 600.0f) / 1200.0f;
    float ny = (contactWorldPos.y + 360.0f) / 720.0f;
    Vector2 contactScreenPos = {
        sonarBounds.x + nx * sonarBounds.width,
        sonarBounds.y + ny * sonarBounds.height
    };
    
    crosshairManager.updateMousePosition(contactScreenPos, sonarBounds);
    crosshairManager.handleMouseClick(contactScreenPos, sonarBounds);
    
    EXPECT_TRUE(crosshairManager.isTracking());
    EXPECT_EQ(crosshairManager.getTrackedContactId(), contactId);
    
    // Update should maintain tracking
    crosshairManager.update(0.016f);
    
    EXPECT_TRUE(crosshairManager.isTracking());
    EXPECT_EQ(crosshairManager.getTrackedContactId(), contactId);
    
    // Clear tracking
    crosshairManager.clearTracking();
    
    EXPECT_FALSE(crosshairManager.isTracking());
    EXPECT_EQ(crosshairManager.getTrackedContactId(), 0);
}

TEST_F(CrosshairManagerTest, ContactTracking) {
    // TODO: Test contact tracking functionality
    EXPECT_TRUE(true); // Placeholder
}
