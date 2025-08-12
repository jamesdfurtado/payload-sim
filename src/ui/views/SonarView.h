#pragma once

#include "../Widget.h"
#include "../../sim/world/ContactManager.h"

class SonarView : public Widget {
public:
    explicit SonarView(ContactManager& contacts) : contacts(contacts) {}

    void draw() const override {
        DrawRectangleRec(bounds, DARKGREEN);
        DrawRectangleLinesEx(bounds, 2, LIME);
        // draw contacts
        for (const auto& c : contacts.getActiveContacts()) {
            float nx = (c.position.x + 600.0f) / 1200.0f;
            float ny = (c.position.y + 360.0f) / 720.0f;
            const int px = (int)(bounds.x + nx * bounds.width);
            const int py = (int)(bounds.y + ny * bounds.height);
            DrawCircle(px, py, 4, (c.type == ContactType::EnemySub) ? RED : SKYBLUE);
        }
    }

    bool onMouseUp(Vector2 pos) override {
        if (!contains(pos)) return false;
        float nx = (pos.x - bounds.x) / bounds.width;
        float ny = (pos.y - bounds.y) / bounds.height;
        Vector2 world{ nx * 1200.0f - 600.0f, ny * 720.0f - 360.0f };
        uint32_t targetId = contacts.getNearestContactId(world, 40.0f);
        if (targetId != 0) {
            contacts.launchMissile(targetId, {0,0});
        }
        return true;
    }

private:
    ContactManager& contacts;
};


