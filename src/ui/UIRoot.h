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
#include "ui/views/ControlPanel.h"
#include "ui/views/ContactView.h"
#include "ui/views/PowerView.h"
#include "ui/views/DepthView.h"

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
        depthView = std::make_unique<DepthView>(engine, *depth);
        controlPanel = std::make_unique<ControlPanel>(safety);
        contactView = std::make_unique<ContactView>(*contacts);

        // Simple layout (manual rects to keep lean)
        sonarView->setBounds({20, 120, 600, 580});
        statusPanel->setBounds({640, 20, 620, 110});
        powerView->setBounds({640, 140, 620, 100});
        depthView->setBounds({640, 250, 620, 100});
        controlPanel->setBounds({640, 360, 620, 340});
    }

    void update(float dt) {
        // Poll safety phase to log simple milestones
        static LaunchPhase previous = LaunchPhase::Idle;
        LaunchPhase current = safety->getPhase();
        if (current != previous) {
            switch (current) {
                case LaunchPhase::Authorized: break;
                case LaunchPhase::Arming: break;
                case LaunchPhase::Armed: break;
                case LaunchPhase::Launching: {
                    // Trigger missile launch using currently selected target
                    uint32_t id = sonar->getSelectedTargetId();
                    if (id != 0) {
                        contacts->launchMissile(id, {0,0});
                    }
                    break; }
                case LaunchPhase::Launched: break;
                case LaunchPhase::Resetting: break;
                case LaunchPhase::Idle: break;
            }
            previous = current;
        }

        // Update all views
        powerView->update(dt);
        depthView->update(dt);
        controlPanel->update(dt);

        // Simple mouse routing
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseDown(mouse)) {} else if (depthView->onMouseDown(mouse)) {} else if (sonarView->onMouseDown(mouse)) {} else if (controlPanel->onMouseDown(mouse)) {}
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseUp(mouse)) {} else if (depthView->onMouseUp(mouse)) {} else if (sonarView->onMouseUp(mouse)) {} else if (controlPanel->onMouseUp(mouse)) {}
        }
        
        // Handle mouse movement for dragging
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseMove(mouse)) {} else if (depthView->onMouseMove(mouse)) {} else if (sonarView->onMouseMove(mouse)) {} else if (controlPanel->onMouseMove(mouse)) {}
        }
    }

    void draw() const {
        DrawText("Submarine Payload Launch Control Simulator", 20, 20, 24, RAYWHITE);
        statusPanel->draw();
        powerView->draw();
        depthView->draw();
        controlPanel->draw();

        // Draw sonar view first (background, grid, crosshair, submarine)
        sonarView->draw();
        
        // Then overlay contacts on top of the sonar display
        contactView->drawContactsOnSonar(sonarView->getBounds());
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
    std::unique_ptr<DepthView> depthView;
    std::unique_ptr<ControlPanel> controlPanel;
    std::unique_ptr<ContactView> contactView;

};


