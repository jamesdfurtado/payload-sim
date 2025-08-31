#pragma once

#include <memory>
#include <string>
#include <raylib.h>
#include "../sim/SimulationEngine.h"
#include "../sim/systems/SonarSystem.h"
#include "../sim/systems/PowerSystem.h"
#include "../sim/systems/DepthSystem.h"
#include "../sim/systems/TargetingSystem.h"
#include "../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "../sim/systems/EnvironmentSystem.h"
#include "../sim/world/ContactManager.h"
#include "../sim/world/CrosshairManager.h"
#include "../sim/world/MissileManager.h"
#include "../sim/world/MissionInstructionManager.h"
#include "ui/views/SonarView.h"
#include "ui/views/MissileView.h"
#include "ui/views/StatusPanel.h"
#include "ui/views/ControlPanel/ControlPanel.h"
#include "ui/views/ContactView.h"
#include "ui/views/CrosshairView.h"
#include "ui/views/PowerView.h"
#include "ui/views/DepthView.h"
#include "ui/views/GuidanceView.h"
#include "ui/widgets/PulsatingBorder.h"

class UIRoot {
public:
    UIRoot(SimulationEngine& engine,
           SonarSystem* sonar,
           PowerSystem* power,
           DepthSystem* depth,
           TargetingSystem* targeting,
           LaunchSequenceHandler* launchSequence,
           EnvironmentSystem* /*environment*/,
           ContactManager* contacts,
           CrosshairManager* crosshair,
           MissileManager* missiles)
        : engine(engine), sonar(sonar), power(power), depth(depth), targeting(targeting), launchSequence(launchSequence), contacts(contacts), crosshairManager(crosshair), missileManager(missiles) {
        
        // Create mission instruction manager
        missionManager = std::make_unique<MissionInstructionManager>(engine, launchSequence);

        sonarView = std::make_unique<SonarView>(*contacts);
        statusPanel = std::make_unique<StatusPanel>(engine);
        powerView = std::make_unique<PowerView>(engine, *power);
        depthView = std::make_unique<DepthView>(engine, *depth);
        controlPanel = std::make_unique<ControlPanel>(engine, launchSequence);
        contactView = std::make_unique<ContactView>(*contacts);
        crosshairView = std::make_unique<CrosshairView>(*crosshairManager);
        missileView = std::make_unique<MissileView>(*missiles);
        guidanceView = std::make_unique<GuidanceView>(*missionManager);
        
        // Create pulsating border for UI highlighting
        uiPulsatingBorder = PulsatingBorder(YELLOW, 4.0f, 0.2f, 1.0f, 3);

        // Simple layout (manual rects to keep lean)
        guidanceView->setBounds({20, 60, 600, 70});  // Position above sonar view with more height
        sonarView->setBounds({20, 140, 600, 560});  // Move down to accommodate taller guidance box
        statusPanel->setBounds({640, 20, 620, 110});
        powerView->setBounds({640, 140, 620, 100});
        depthView->setBounds({640, 250, 620, 100});
        controlPanel->setBounds({640, 360, 620, 340});
    }

    void update(float dt) {
        // Update all views
        guidanceView->update(dt);
        powerView->update(dt);
        depthView->update(dt);
        controlPanel->update(dt);
        crosshairManager->update(dt);
        
        // Update pulsating border for UI highlighting
        uiPulsatingBorder.update(dt);

        // Update crosshair manager with mouse position
        Vector2 mouse = GetMousePosition();
        crosshairManager->updateMousePosition(mouse, sonarView->getBounds());

        // Simple mouse routing
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseDown(mouse)) {} else if (depthView->onMouseDown(mouse)) {} else if (sonarView->onMouseDown(mouse)) {} else if (controlPanel->onMouseDown(mouse)) {}
            
            // Handle crosshair selection
            crosshairManager->handleMouseClick(mouse, sonarView->getBounds());
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
        guidanceView->draw();
        statusPanel->draw();
        
        // Draw power view with potential pulsating border around the entire panel
        if (missionManager->shouldPulsate(PulsateTarget::POWER_SWITCH)) {
            drawPulsatingBorder(powerView->getBounds());
        }
        powerView->draw();
        
        // Draw depth view with potential pulsating border around the entire panel
        if (missionManager->shouldPulsate(PulsateTarget::DEPTH_THROTTLE)) {
            drawPulsatingBorder(depthView->getBounds());
        }
        depthView->draw();
        
        // Draw control panel with potential pulsating borders for different elements
        drawControlPanelWithPulsation();
        
        // Draw sonar view with potential pulsating border
        if (missionManager->shouldPulsate(PulsateTarget::SONAR_BOX)) {
            drawPulsatingBorder(sonarView->getBounds());
        }
        // Draw sonar view first (background, grid, submarine)
        sonarView->draw();
        
        // Then overlay contacts on top of the sonar display
        contactView->drawContactsOnSonar(sonarView->getBounds());
        
        // Overlay missiles and explosions
        missileView->drawMissilesOnSonar(sonarView->getBounds());
        
        // Finally overlay crosshair and selection circle
        crosshairView->drawOnSonar(sonarView->getBounds());
        

    }

private:
    SimulationEngine& engine;
    MissileManager* missileManager;
    SonarSystem* sonar;
    PowerSystem* power;
    DepthSystem* depth;
    TargetingSystem* targeting;
    LaunchSequenceHandler* launchSequence;
    ContactManager* contacts;

    std::unique_ptr<SonarView> sonarView;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<PowerView> powerView;
    std::unique_ptr<DepthView> depthView;
    std::unique_ptr<ControlPanel> controlPanel;
    std::unique_ptr<ContactView> contactView;
    CrosshairManager* crosshairManager;
    std::unique_ptr<CrosshairView> crosshairView;
    std::unique_ptr<MissileView> missileView;
    std::unique_ptr<GuidanceView> guidanceView;
    
    // Mission instruction manager (simulation layer)
    std::unique_ptr<MissionInstructionManager> missionManager;
    
    // Pulsating border for UI highlighting
    PulsatingBorder uiPulsatingBorder;
    
    // Helper methods for pulsating borders
    void drawPulsatingBorder(const Rectangle& bounds) const {
        uiPulsatingBorder.drawBorder(bounds);
    }
    
    void drawControlPanelWithPulsation() const {
        // Draw control panel normally
        controlPanel->draw();
        
        // Draw pulsating borders for specific control panel elements based on exact layout calculations
        if (missionManager->shouldPulsate(PulsateTarget::AUTHORIZE_BUTTON)) {
            // Replicate exact calculation from ControlPanel and LaunchSequencePanel
            Rectangle controlBounds = controlPanel->getBounds();
            float margin = 15;
            float leftWidth = controlBounds.width * 0.35f;
            float buttonHeight = 35;
            float buttonWidth = leftWidth - 2 * margin;
            
            // Left panel area (from ControlPanel::setupLayout)
            Rectangle leftPanelArea;
            leftPanelArea.x = controlBounds.x + margin;
            leftPanelArea.y = controlBounds.y + margin;
            leftPanelArea.width = leftWidth;
            leftPanelArea.height = controlBounds.height - 2 * margin;
            
            // Button area (from LaunchSequencePanel::setupLayout)
            Rectangle buttonArea;
            buttonArea.x = leftPanelArea.x + margin;
            buttonArea.y = leftPanelArea.y + margin;
            
            // First button: authorize (from LaunchSequencePanel line 42)
            float buttonY = buttonArea.y + 80; // 80px offset
            Rectangle authBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(authBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::ARM_BUTTON)) {
            // Replicate exact calculation for second button
            Rectangle controlBounds = controlPanel->getBounds();
            float margin = 15;
            float leftWidth = controlBounds.width * 0.35f;
            float buttonHeight = 35;
            float buttonWidth = leftWidth - 2 * margin;
            
            Rectangle leftPanelArea;
            leftPanelArea.x = controlBounds.x + margin;
            leftPanelArea.y = controlBounds.y + margin;
            leftPanelArea.width = leftWidth;
            leftPanelArea.height = controlBounds.height - 2 * margin;
            
            Rectangle buttonArea;
            buttonArea.x = leftPanelArea.x + margin;
            buttonArea.y = leftPanelArea.y + margin;
            
            // Second button: arm (from LaunchSequencePanel lines 42-45)
            float buttonY = buttonArea.y + 80; // Start position
            buttonY += buttonHeight + margin;   // Move to second button
            Rectangle armBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(armBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::LAUNCH_BUTTON)) {
            // Replicate exact calculation for third button
            Rectangle controlBounds = controlPanel->getBounds();
            float margin = 15;
            float leftWidth = controlBounds.width * 0.35f;
            float buttonHeight = 35;
            float buttonWidth = leftWidth - 2 * margin;
            
            Rectangle leftPanelArea;
            leftPanelArea.x = controlBounds.x + margin;
            leftPanelArea.y = controlBounds.y + margin;
            leftPanelArea.width = leftWidth;
            leftPanelArea.height = controlBounds.height - 2 * margin;
            
            Rectangle buttonArea;
            buttonArea.x = leftPanelArea.x + margin;
            buttonArea.y = leftPanelArea.y + margin;
            
            // Third button: launch (from LaunchSequencePanel lines 42-48)
            float buttonY = buttonArea.y + 80; // Start position
            buttonY += buttonHeight + margin;   // Move to second button
            buttonY += buttonHeight + margin;   // Move to third button
            Rectangle launchBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(launchBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::KEYPAD_AREA)) {
            // Calculate keypad area bounds using exact same logic as ControlPanel
            Rectangle controlBounds = controlPanel->getBounds();
            float margin = 15;
            float leftWidth = controlBounds.width * 0.35f;
            float rightWidth = controlBounds.width * 0.35f;
            float centerGap = controlBounds.width * 0.20f;
            
            // Right panel area calculation
            float rightPanelX = controlBounds.x + leftWidth + centerGap + margin;
            float rightPanelY = controlBounds.y + margin;
            
            // Auth area height
            float authAreaHeight = 100;
            
            // Keypad area calculation (matches ControlPanel::setupLayout)
            float keypadY = rightPanelY + authAreaHeight + margin - 20; // Move keypad up by 20px
            float keypadWidth = 120;
            float keypadHeight = 160;
            float keypadX = rightPanelX + (rightWidth - keypadWidth) / 2; // Centered in right panel
            
            // Adjust bounds: expand top and right borders more
            Rectangle keypadBounds = {
                keypadX - 15,           // Left padding
                keypadY - 25,           // Expand top border more (was -15, now -25)
                keypadWidth + 50,       // Expand right border more (was +40, now +50)
                keypadHeight + 60       // Keep current height
            };
            drawPulsatingBorder(keypadBounds);
        }
    }

};


