#pragma once

#include "../Widget.h"
#include "../../sim/world/ContactManager.h"

class ContactView : public Widget {
public:
    explicit ContactView(ContactManager& contacts) : contacts(contacts) {}

    void draw() const override {
        // This view doesn't draw its own panel - it only renders contacts on sonar
    }

    // This method renders contacts on the sonar display
    void drawContactsOnSonar(const Rectangle& sonarBounds) const {
        for (const auto& contact : contacts.getActiveContacts()) {
            Vector2 screen = worldToScreen(contact.position, sonarBounds);
            Color contactColor = getContactTypeColor(contact.type);
            
            // Draw simple colored dot - no outlines, no numbers, same size
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
    // Coordinate transform: world (-600..600, -360..360) to screen inside rectangle
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
