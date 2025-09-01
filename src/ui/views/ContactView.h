#pragma once

#include "../Widget.h"
#include "../../sim/world/ContactManager.h"

class ContactView : public Widget {
public:
    explicit ContactView(ContactManager& contacts) : contacts(contacts) {}

//    void draw() const override {
 //   }

    // draws contact dots on sonar
    void drawContactsOnSonar(const Rectangle& sonarBounds) const {
        for (const auto& contact : contacts.getActiveContacts()) {
            Vector2 screen = worldToScreen(contact.position, sonarBounds);
            Color contactColor = getContactTypeColor(contact.type);
            DrawCircle((int)screen.x, (int)screen.y, 4, contactColor);
        }
    }

    bool onMouseDown(Vector2 pos) override {
        return contains(pos);
    }

    bool onMouseUp(Vector2 pos) override {
        return contains(pos);
    }

    bool onMouseMove(Vector2 pos) override {
        return contains(pos);
    }

private:
    // convert world coords to screen pixels
    static Vector2 worldToScreen(Vector2 world, const Rectangle& r) {
        float nx = (world.x + 600.0f) / 1200.0f;
        float ny = (world.y + 360.0f) / 720.0f;
        return { r.x + nx * r.width, r.y + ny * r.height };
    }

    Color getContactTypeColor(ContactType type) const {
        switch (type) {
            case ContactType::EnemySub: return RED;
            case ContactType::FriendlySub: return GREEN;
            case ContactType::Fish: return SKYBLUE;
            case ContactType::Debris: return GRAY;
            default: return WHITE;
        }
    }

    ContactManager& contacts;
};
