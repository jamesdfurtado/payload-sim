#include <gtest/gtest.h>
#include "ui/views/ContactView.h"
#include "sim/world/ContactManager.h"

class MockContactManager : public ContactManager {
public:
    MockContactManager() : ContactManager(nullptr) {}
    void update(SimulationState& state, float dt) override {}
    
    std::vector<Contact> contacts;
    
    const std::vector<Contact>& getActiveContacts() const override { return contacts; }
    void addContact(Contact contact) { contacts.push_back(contact); }
    void clearContacts() { contacts.clear(); }
};

class ContactViewTest : public ::testing::Test {
protected:
    std::unique_ptr<MockContactManager> contactManager;
    std::unique_ptr<ContactView> contactView;
    
    void SetUp() override {
        contactManager = std::make_unique<MockContactManager>();
        contactView = std::make_unique<ContactView>(*contactManager);
        contactView->setBounds({10, 10, 600, 400});
    }
};

TEST_F(ContactViewTest, WorldToScreenConvertsCoordinatesCorrectly) {
    Vector2 worldPos = {0, 0};
    Rectangle bounds = {10, 10, 600, 400};
    
    Vector2 screenPos = contactView->worldToScreen(worldPos, bounds);
    
    EXPECT_FLOAT_EQ(screenPos.x, 10 + 600/2);
    EXPECT_FLOAT_EQ(screenPos.y, 10 + 400/2);
}

TEST_F(ContactViewTest, WorldToScreenConvertsNegativeCoordinates) {
    Vector2 worldPos = {-600, -360};
    Rectangle bounds = {10, 10, 600, 400};
    
    Vector2 screenPos = contactView->worldToScreen(worldPos, bounds);
    
    EXPECT_FLOAT_EQ(screenPos.x, 10);
    EXPECT_FLOAT_EQ(screenPos.y, 10);
}

TEST_F(ContactViewTest, WorldToScreenConvertsPositiveCoordinates) {
    Vector2 worldPos = {600, 360};
    Rectangle bounds = {10, 10, 600, 400};
    
    Vector2 screenPos = contactView->worldToScreen(worldPos, bounds);
    
    EXPECT_FLOAT_EQ(screenPos.x, 10 + 600);
    EXPECT_FLOAT_EQ(screenPos.y, 10 + 400);
}

TEST_F(ContactViewTest, DrawContactsOnSonarRendersCorrectly) {
    Contact enemyContact = {{100, 50}, ContactType::EnemySub};
    Contact friendlyContact = {{-200, 100}, ContactType::FriendlySub};
    Contact fishContact = {{0, 0}, ContactType::Fish};
    
    contactManager->addContact(enemyContact);
    contactManager->addContact(friendlyContact);
    contactManager->addContact(fishContact);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    EXPECT_TRUE(contactView->hasDrawn());
    EXPECT_EQ(contactView->getLastDrawnContactCount(), 3);
}

TEST_F(ContactViewTest, DrawContactsOnSonarPositionsCorrectly) {
    Contact contact = {{0, 0}, ContactType::EnemySub};
    contactManager->addContact(contact);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    Vector2 expectedScreenPos = {10 + 600/2, 10 + 400/2};
    EXPECT_EQ(contactView->getLastDrawnContactPosition(0).x, expectedScreenPos.x);
    EXPECT_EQ(contactView->getLastDrawnContactPosition(0).y, expectedScreenPos.y);
}

TEST_F(ContactViewTest, DrawContactsOnSonarHandlesMultipleContacts) {
    Contact contact1 = {{100, 50}, ContactType::EnemySub};
    Contact contact2 = {{-200, 100}, ContactType::FriendlySub};
    Contact contact3 = {{0, 0}, ContactType::Fish};
    
    contactManager->addContact(contact1);
    contactManager->addContact(contact2);
    contactManager->addContact(contact3);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    EXPECT_EQ(contactView->getLastDrawnContactCount(), 3);
    EXPECT_EQ(contactView->getLastDrawnContactColor(0).r, RED.r);
    EXPECT_EQ(contactView->getLastDrawnContactColor(1).r, GREEN.r);
    EXPECT_EQ(contactView->getLastDrawnContactColor(2).r, SKYBLUE.r);
}

TEST_F(ContactViewTest, DrawContactsOnSonarHandlesEmptyContactList) {
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    EXPECT_EQ(contactView->getLastDrawnContactCount(), 0);
}

TEST_F(ContactViewTest, MouseEventsReturnTrueWhenInsideBounds) {
    Vector2 insidePos = {100, 100};
    
    EXPECT_TRUE(contactView->onMouseDown(insidePos));
    EXPECT_TRUE(contactView->onMouseUp(insidePos));
    EXPECT_TRUE(contactView->onMouseMove(insidePos));
}

TEST_F(ContactViewTest, MouseEventsReturnFalseWhenOutsideBounds) {
    Vector2 outsidePos = {1000, 1000};
    
    EXPECT_FALSE(contactView->onMouseDown(outsidePos));
    EXPECT_FALSE(contactView->onMouseUp(outsidePos));
    EXPECT_FALSE(contactView->onMouseMove(outsidePos));
}

TEST_F(ContactViewTest, DrawContactsOnSonarUsesCorrectRadius) {
    Contact contact = {{0, 0}, ContactType::EnemySub};
    contactManager->addContact(contact);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    EXPECT_EQ(contactView->getLastDrawnContactRadius(), 4);
}

TEST_F(ContactViewTest, CoordinateConversionIsConsistent) {
    Vector2 worldPos = {300, 180};
    Rectangle bounds = {10, 10, 600, 400};
    
    Vector2 screenPos = contactView->worldToScreen(worldPos, bounds);
    
    float expectedX = 10 + (300 + 600.0f) / 1200.0f * 600;
    float expectedY = 10 + (180 + 360.0f) / 720.0f * 400;
    
    EXPECT_FLOAT_EQ(screenPos.x, expectedX);
    EXPECT_FLOAT_EQ(screenPos.y, expectedY);
}

TEST_F(ContactViewTest, MouseEventsWorkWithDifferentBounds) {
    contactView->setBounds({20, 20, 800, 600});
    
    Vector2 insidePos = {100, 100};
    Vector2 outsidePos = {1000, 1000};
    
    EXPECT_TRUE(contactView->onMouseDown(insidePos));
    EXPECT_TRUE(contactView->onMouseUp(insidePos));
    EXPECT_TRUE(contactView->onMouseMove(insidePos));
    
    EXPECT_FALSE(contactView->onMouseDown(outsidePos));
    EXPECT_FALSE(contactView->onMouseUp(outsidePos));
    EXPECT_FALSE(contactView->onMouseMove(outsidePos));
}

TEST_F(ContactViewTest, DrawContactsOnSonarHandlesNegativeWorldCoordinates) {
    Contact contact = {{-300, -180}, ContactType::EnemySub};
    contactManager->addContact(contact);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    float expectedX = 10 + (-300 + 600.0f) / 1200.0f * 600;
    float expectedY = 10 + (-180 + 360.0f) / 720.0f * 400;
    
    EXPECT_FLOAT_EQ(contactView->getLastDrawnContactPosition(0).x, expectedX);
    EXPECT_FLOAT_EQ(contactView->getLastDrawnContactPosition(0).y, expectedY);
}

TEST_F(ContactViewTest, DrawContactsOnSonarHandlesPositiveWorldCoordinates) {
    Contact contact = {{300, 180}, ContactType::EnemySub};
    contactManager->addContact(contact);
    
    Rectangle sonarBounds = {10, 10, 600, 400};
    contactView->drawContactsOnSonar(sonarBounds);
    
    float expectedX = 10 + (300 + 600.0f) / 1200.0f * 600;
    float expectedY = 10 + (180 + 360.0f) / 720.0f * 400;
    
    EXPECT_FLOAT_EQ(contactView->getLastDrawnContactPosition(0).x, expectedX);
    EXPECT_FLOAT_EQ(contactView->getLastDrawnContactPosition(0).y, expectedY);
}
