#pragma once

#include "../../Widget.h"
#include "LaunchSequencePanel.h"
#include "KeypadPanel.h"
#include "AuthCodePanel.h"
#include "LaunchPhaseDisplay.h"
#include "../../../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "../../../sim/SimulationEngine.h"
#include <memory>
#include <raylib.h>

class ControlPanel : public Widget {
public:
    explicit ControlPanel(SimulationEngine& engine, LaunchSequenceHandler* launchSequence);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt) override;
    void setBounds(Rectangle newBounds);

private:
    // launch sequence logic
    LaunchSequenceHandler* sequenceHandler;
    
    // child panels
    std::unique_ptr<LaunchSequencePanel> launchSequencePanel;
    std::unique_ptr<KeypadPanel> keypadPanel;
    std::unique_ptr<AuthCodePanel> authCodePanel;
    std::unique_ptr<LaunchPhaseDisplay> phaseDisplay;
    
    // panel areas
    Rectangle leftPanelArea;
    Rectangle rightPanelArea;
    
    // internal helpers
    void setupLayout();
    void handleKeypadInput(char key);
    void handleBackspace();
    void handleAuthCodeSubmit(const std::string& code);
    
    // button handlers
    void onAuthorize();
};
