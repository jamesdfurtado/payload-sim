#pragma once

#include <memory>
#include <string>
#include <raylib.h>
#include "../sim/SimulationEngine.h"
#include "../sim/systems/SonarSystem.h"
#include "../sim/systems/PowerSystem.h"
#include "../sim/systems/DepthSystem.h"
#include "../sim/systems/TargetingSystem.h"
#include "../sim/systems/SafetySystem.h"
#include "../sim/systems/EnvironmentSystem.h"
#include "../sim/world/ContactManager.h"
#include "ui/views/SonarView.h"
#include "ui/views/StatusPanel.h"
#include "ui/views/LogPanel.h"
#include "ui/views/PowerView.h"

class UIRoot {
public:
    UIRoot(SimulationEngine& engine,
           SonarSystem* sonar,
           PowerSystem* power,
           DepthSystem* depth,
           TargetingSystem* targeting,
           SafetySystem* safety,
           EnvironmentSystem* /*environment*/,
           ContactManager* contacts)
        : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), safety(safety), contacts(contacts) {

        sonarView = std::make_unique<SonarView>(*contacts);
        statusPanel = std::make_unique<StatusPanel>(engine);
        powerView = std::make_unique<PowerView>(engine, *power);
        logPanel = std::make_unique<LogPanel>();

        // Simple layout (manual rects to keep lean)
        sonarView->setBounds({20, 120, 600, 580});
        statusPanel->setBounds({640, 20, 620, 110});
        powerView->setBounds({640, 140, 620, 100});
        logPanel->setBounds({640, 250, 620, 120});

        // Wire sonar selection to selecting a target in the sonar system
        sonarView->setOnSelect([this](Vector2 world){ this->sonar->attemptManualLock(world); });
    }

    void update(float /*dt*/) {
        // Poll safety phase to log simple milestones
        static LaunchPhase previous = LaunchPhase::Idle;
        LaunchPhase current = safety->getPhase();
        if (current != previous) {
            switch (current) {
                case LaunchPhase::Authorized: logPanel->add("Authorized"); break;
                case LaunchPhase::Arming: logPanel->add("Arming..."); break;
                case LaunchPhase::Armed: logPanel->add("Armed"); break;
                case LaunchPhase::Launching: {
                    logPanel->add("Launching...");
                    // Trigger missile launch using currently selected target
                    uint32_t id = sonar->getSelectedTargetId();
                    if (id != 0) {
                        contacts->launchMissile(id, {0,0});
                    }
                    break; }
                case LaunchPhase::Launched: logPanel->add("Launched!"); break;
                case LaunchPhase::Resetting: logPanel->add("Resetting..."); break;
                case LaunchPhase::Idle: logPanel->add("Idle"); break;
            }
            previous = current;
        }

        // Simple mouse routing
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseDown(mouse)) {} else if (sonarView->onMouseDown(mouse)) {}
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseUp(mouse)) {} else if (sonarView->onMouseUp(mouse)) {}
        }
    }

    void draw() const {
        DrawText("Submarine Payload Launch Control Simulator", 20, 20, 24, RAYWHITE);
        statusPanel->draw();
        powerView->draw();
        sonarView->draw();
        logPanel->draw();
    }

private:
    SimulationEngine& engine;
    SonarSystem* sonar;
    PowerSystem* power;
    DepthSystem* depth;
    TargetingSystem* targeting;
    SafetySystem* safety;
    ContactManager* contacts;

    std::unique_ptr<SonarView> sonarView;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<PowerView> powerView;
    std::unique_ptr<LogPanel> logPanel;
};


