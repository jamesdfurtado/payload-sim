#pragma once

#include "../Widget.h"
#include "../widgets/Button.h"
#include "../widgets/Slider.h"
#include "../../sim/systems/SafetySystem.h"
#include "../../sim/systems/PowerSystem.h"
#include "../../sim/systems/DepthSystem.h"
#include "../../sim/systems/TargetingSystem.h"
#include <memory>

class ControlsPanel : public Widget {
public:
    ControlsPanel(SafetySystem& safety, PowerSystem& power, DepthSystem& depth, TargetingSystem& targeting)
        : safety(safety), power(power), depth(depth), targeting(targeting) {
        // Buttons
        btnAuth = std::make_unique<Button>("Authorize", [this]{ this->safety.requestAuthorizationCode(this->currentCode.c_str()); });
        btnArm = std::make_unique<Button>("Arm", [this]{ this->safety.arm(); });
        btnLaunch = std::make_unique<Button>("Launch", [this]{ this->safety.launch(); });
        btnReset = std::make_unique<Button>("Reset", [this]{ this->safety.reset(); });

        // Sliders
        sldPower = std::make_unique<Slider>([this](float v){ this->power.setPowerLevel(v); }, 0.0f);
        sldDepth = std::make_unique<Slider>([this](float v){ this->depth.setDepth(50.0f + v * 300.0f); }, 0.2f);
        sldStability = std::make_unique<Slider>([this](float v){ this->targeting.adjustStability(v - this->lastStability); this->lastStability = v; }, 0.5f);
        lastStability = 0.5f;
        currentCode = "1234"; // simple placeholder
    }

    void update(float /*dt*/) override {}

    void draw() const override {
        DrawRectangleRec(bounds, Fade(DARKBROWN, 0.4f));
        DrawRectangleLinesEx(bounds, 2, ORANGE);
        int x = (int)bounds.x + 10;
        int y = (int)bounds.y + 8;
        DrawText("Controls", x, y, 20, RAYWHITE); y += 26;
        // Layout
        Rectangle rBtn{ (float)x, (float)y, 120, 36 };
        btnAuth->setBounds(rBtn); btnAuth->draw(); rBtn.y += 40;
        btnArm->setBounds(rBtn); btnArm->draw(); rBtn.y += 40;
        btnLaunch->setBounds(rBtn); btnLaunch->draw(); rBtn.y += 40;
        btnReset->setBounds(rBtn); btnReset->draw();

        int sx = x + 160; int sy = y;
        DrawText("Power", sx, sy, 18, RAYWHITE); sy += 22;
        Rectangle rs{ (float)sx, (float)sy, bounds.width - 180.0f, 20 }; sldPower->setBounds(rs); sldPower->draw(); sy += 34;
        DrawText("Depth", sx, sy, 18, RAYWHITE); sy += 22;
        rs.y = (float)sy; sldDepth->setBounds(rs); sldDepth->draw(); sy += 34;
        DrawText("Stability", sx, sy, 18, RAYWHITE); sy += 22;
        rs.y = (float)sy; sldStability->setBounds(rs); sldStability->draw();

        // Show current auth code (temporary simplification)
        DrawText("Auth code: 1234 (placeholder)", x, (int)bounds.y + (int)bounds.height - 28, 18, SKYBLUE);
    }

    bool onMouseDown(Vector2 pos) override {
        return btnAuth->onMouseDown(pos) || btnArm->onMouseDown(pos) || btnLaunch->onMouseDown(pos) || btnReset->onMouseDown(pos)
            || sldPower->onMouseDown(pos) || sldDepth->onMouseDown(pos) || sldStability->onMouseDown(pos);
    }
    bool onMouseUp(Vector2 pos) override {
        return btnAuth->onMouseUp(pos) || btnArm->onMouseUp(pos) || btnLaunch->onMouseUp(pos) || btnReset->onMouseUp(pos)
            || sldPower->onMouseUp(pos) || sldDepth->onMouseUp(pos) || sldStability->onMouseUp(pos);
    }

private:
    SafetySystem& safety;
    PowerSystem& power;
    DepthSystem& depth;
    TargetingSystem& targeting;

    std::unique_ptr<Button> btnAuth, btnArm, btnLaunch, btnReset;
    std::unique_ptr<Slider> sldPower, sldDepth, sldStability;
    std::string currentCode;
    float lastStability = 0.5f;
};


