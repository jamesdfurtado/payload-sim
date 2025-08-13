#pragma once

#include "../Widget.h"
#include "../widgets/Button.h"
#include "../widgets/InputBox.h"
#include "../../sim/systems/SafetySystem.h"
#include <memory>
#include <vector>
#include <string>
#include <raylib.h>

class ControlPanel : public Widget {
public:
    explicit ControlPanel(SafetySystem* safetySystem);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt);
    void setBounds(Rectangle newBounds);

private:
    SafetySystem* safetySystem;
    
    // State buttons
    std::unique_ptr<Button> authorizeButton;
    std::unique_ptr<Button> armButton;
    std::unique_ptr<Button> launchButton;
    std::unique_ptr<Button> resetButton;
    
    // Authentication system
    std::unique_ptr<InputBox> authCodeInput;
    std::unique_ptr<InputBox> authCodeDisplay;
    
    // Keypad buttons
    std::vector<std::unique_ptr<Button>> keypadButtons;
    
    // Layout
    Rectangle buttonArea;
    Rectangle keypadArea;
    Rectangle authArea;
    Rectangle stateDisplayArea;
    
    // State tracking
    std::string currentAuthCode;
    bool authCodeEntered;
    
    // Helper methods
    void setupLayout();
    void createKeypad();
    void updateButtonStates();
    void handleKeypadInput(char key);
    void handleAuthCodeSubmit();
    std::string getPhaseString(LaunchPhase phase) const;
    Color getPhaseColor(LaunchPhase phase) const;
    
    // Button callbacks
    void onAuthorize();
    void onArm();
    void onLaunch();
    void onReset();
};
