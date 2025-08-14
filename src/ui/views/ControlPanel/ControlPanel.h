#pragma once

#include "../../Widget.h"
#include "LaunchSequencePanel.h"
#include "KeypadPanel.h"
#include "AuthCodePanel.h"
#include "../../../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "../../../sim/SimulationEngine.h"
#include <memory>
#include <raylib.h>

class ControlPanel : public Widget {
public:
    explicit ControlPanel(SimulationEngine& engine);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt);
    void setBounds(Rectangle newBounds);

private:
    // Launch sequence handler
    std::unique_ptr<LaunchSequenceHandler> sequenceHandler;
    
    // Sub-panels
    std::unique_ptr<LaunchSequencePanel> launchSequencePanel;
    std::unique_ptr<KeypadPanel> keypadPanel;
    std::unique_ptr<AuthCodePanel> authCodePanel;
    
    // Layout
    Rectangle leftPanelArea;
    Rectangle rightPanelArea;
    
    // State tracking
    std::string currentAuthCode;
    bool authCodeEntered;
    
    // Helper methods
    void setupLayout();
    void handleKeypadInput(char key);
    void handleBackspace();
    void handleAuthCodeSubmit(const std::string& code);
    
    // Button callbacks
    void onAuthorize();
};
