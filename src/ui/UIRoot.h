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
        
        // mission instructions drive the ui flow
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
        
        uiPulsatingBorder = PulsatingBorder(YELLOW, 4.0f, 0.2f, 1.0f, 3);

        guidanceView->setBounds({20, 60, 600, 70});
        sonarView->setBounds({20, 140, 600, 560});
        statusPanel->setBounds({640, 20, 620, 110});
        powerView->setBounds({640, 140, 620, 100});
        depthView->setBounds({640, 250, 620, 100});
        controlPanel->setBounds({640, 360, 620, 340});
    }

    void update(float dt) {
        guidanceView->update(dt);
        powerView->update(dt);
        depthView->update(dt);
        controlPanel->update(dt);
        crosshairManager->update(dt);
        
        uiPulsatingBorder.update(dt);

        // track mouse
        Vector2 mouse = GetMousePosition();
        crosshairManager->updateMousePosition(mouse, sonarView->getBounds());

        // basic click handling
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseDown(mouse)) {} else if (depthView->onMouseDown(mouse)) {} else if (sonarView->onMouseDown(mouse)) {} else if (controlPanel->onMouseDown(mouse)) {}
            
            crosshairManager->handleMouseClick(mouse, sonarView->getBounds());
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseUp(mouse)) {} else if (depthView->onMouseUp(mouse)) {} else if (sonarView->onMouseUp(mouse)) {} else if (controlPanel->onMouseUp(mouse)) {}
        }
        
        // mouse dragging support
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (powerView->onMouseMove(mouse)) {} else if (depthView->onMouseMove(mouse)) {} else if (sonarView->onMouseMove(mouse)) {} else if (controlPanel->onMouseMove(mouse)) {}
        }
    }

    void draw() const {
        DrawText("Submarine Payload Launch Control Simulator", 20, 20, 24, RAYWHITE);
        guidanceView->draw();
        statusPanel->draw();
        
        // power controls
        if (missionManager->shouldPulsate(PulsateTarget::POWER_SWITCH)) {
            drawPulsatingBorder(powerView->getBounds());
        }
        powerView->draw();
        
        // depth controls
        if (missionManager->shouldPulsate(PulsateTarget::DEPTH_THROTTLE)) {
            drawPulsatingBorder(depthView->getBounds());
        }
        depthView->draw();
        
        // launch controls
        drawControlPanelWithPulsation();
        
        // sonar display
        if (missionManager->shouldPulsate(PulsateTarget::SONAR_BOX)) {
            drawPulsatingBorder(sonarView->getBounds());
        }
        sonarView->draw();
        
        contactView->drawContactsOnSonar(sonarView->getBounds());
        
        // missile display
        missileView->drawMissilesOnSonar(sonarView->getBounds());
        
        // crosshair
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
    std::unique_ptr<MissionInstructionManager> missionManager;
    
    PulsatingBorder uiPulsatingBorder;
    
    // draw pulsating border effect
    void drawPulsatingBorder(const Rectangle& bounds) const {
        uiPulsatingBorder.drawBorder(bounds);
    }
    
    void drawControlPanelWithPulsation() const {
        // base control panel
        controlPanel->draw();
        
        // pulsate stages of mission flow
        if (missionManager->shouldPulsate(PulsateTarget::AUTHORIZE_BUTTON)) {
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
            
            // auth button location
            float buttonY = buttonArea.y + 80;
            Rectangle authBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(authBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::ARM_BUTTON)) {
            // arm button location
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
            
            float buttonY = buttonArea.y + 80; 
            buttonY += buttonHeight + margin;
            Rectangle armBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(armBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::LAUNCH_BUTTON)) {
            // launch button location
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
            
            // final button location
            float buttonY = buttonArea.y + 80;
            buttonY += buttonHeight + margin;
            buttonY += buttonHeight + margin;
            Rectangle launchBounds = {buttonArea.x, buttonY, buttonWidth, buttonHeight};
            drawPulsatingBorder(launchBounds);
        }
        
        if (missionManager->shouldPulsate(PulsateTarget::KEYPAD_AREA)) {
            Rectangle controlBounds = controlPanel->getBounds();
            float margin = 15;
            float leftWidth = controlBounds.width * 0.35f;
            float rightWidth = controlBounds.width * 0.35f;
            float centerGap = controlBounds.width * 0.20f;
            
            float rightPanelX = controlBounds.x + leftWidth + centerGap + margin;
            float rightPanelY = controlBounds.y + margin;
        
            float authAreaHeight = 100;
            
            // keypad positioning
            float keypadY = rightPanelY + authAreaHeight + margin - 20;
            float keypadWidth = 120;
            float keypadHeight = 160;
            float keypadX = rightPanelX + (rightWidth - keypadWidth) / 2;
            
            Rectangle keypadBounds = {
                keypadX - 15,
                keypadY - 25,
                keypadWidth + 50,
                keypadHeight + 60
            };
            drawPulsatingBorder(keypadBounds);
        }
    }

};


