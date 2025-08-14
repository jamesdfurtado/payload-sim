#include "ControlPanel.h"
#include "../../../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"

ControlPanel::ControlPanel(SimulationEngine& engine) 
    : authCodeEntered(false) {
    
// Create launch sequence handler
sequenceHandler = std::make_unique<LaunchSequenceHandler>(engine);
    
    // Create sub-panels
    launchSequencePanel = std::make_unique<LaunchSequencePanel>(sequenceHandler.get());
    keypadPanel = std::make_unique<KeypadPanel>(
        [this](char key) { authCodePanel->handleKeypadInput(key); },
        [this]() { authCodePanel->handleBackspace(); }
    );
    authCodePanel = std::make_unique<AuthCodePanel>(
        [this](const std::string& code) { handleAuthCodeSubmit(code); }
    );
    
    // Create phase display
    phaseDisplay = std::make_unique<LaunchPhaseDisplay>();
    
    // Set initial bounds and layout
    setBounds(Rectangle{0, 0, 400, 600});
}

void ControlPanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void ControlPanel::setupLayout() {
    // Calculate layout areas for horizontal layout
    float margin = 15;
    float leftWidth = bounds.width * 0.35f;  // Left side for launch sequence
    float rightWidth = bounds.width * 0.35f; // Right side for auth and keypad
    float centerGap = bounds.width * 0.20f;  // Center gap for separation
    
    // Left panel area (launch sequence)
    leftPanelArea.x = bounds.x + margin;
    leftPanelArea.y = bounds.y + margin;
    leftPanelArea.width = leftWidth;
    leftPanelArea.height = bounds.height - 2 * margin;
    
    // Right panel area (auth + keypad)
    rightPanelArea.x = bounds.x + leftWidth + centerGap + margin;
    rightPanelArea.y = bounds.y + margin;
    rightPanelArea.width = rightWidth;
    rightPanelArea.height = bounds.height - 2 * margin;
    
    // Set bounds for sub-panels
    launchSequencePanel->setBounds(leftPanelArea);
    
    // Set bounds for phase display (below launch sequence panel)
    Rectangle phaseDisplayArea = leftPanelArea;
    phaseDisplayArea.y = leftPanelArea.y + leftPanelArea.height - 50; // Position at bottom of left panel
    phaseDisplayArea.height = 50;
    phaseDisplay->setBounds(phaseDisplayArea);
    
    // Split right panel between auth and keypad
    Rectangle authArea = rightPanelArea;
    authArea.height = 100;
    
    Rectangle keypadArea = rightPanelArea;
    keypadArea.y = authArea.y + authArea.height + margin - 20; // Move keypad up by 20px
    keypadArea.height = 160; // Fixed height for keypad
    keypadArea.width = 120;  // Fixed width for keypad
    // Center the keypad horizontally in the right panel
    keypadArea.x = rightPanelArea.x + (rightPanelArea.width - keypadArea.width) / 2;
    
    authCodePanel->setBounds(authArea);
    keypadPanel->setBounds(keypadArea);
}

void ControlPanel::handleAuthCodeSubmit(const std::string& code) {
    currentAuthCode = code;
    if (code == "1511") {
        authCodeEntered = true;
        // TODO: Integrate with LaunchSequenceHandler if needed
    }
}

void ControlPanel::update(float dt) {
    // Update phase display with current phase
    if (sequenceHandler) {
        phaseDisplay->setCurrentPhase(sequenceHandler->getCurrentPhaseString());
    }
    
    // Update all sub-panels
    launchSequencePanel->update(dt);
    keypadPanel->update(dt);
    authCodePanel->update(dt);
}

void ControlPanel::draw() const {
    // Draw background
    DrawRectangleRec(bounds, {15, 15, 15, 255});
    
    // Draw title
    DrawText("CONTROL PANEL", bounds.x + 20, bounds.y + 10, 20, RAYWHITE);

    // Draw all sub-panels
    launchSequencePanel->draw();
    keypadPanel->draw();
    authCodePanel->draw();
    
    // Draw phase display
    phaseDisplay->draw();
}

bool ControlPanel::onMouseDown(Vector2 mousePos) {
    // Delegate to sub-panels
    if (launchSequencePanel->onMouseDown(mousePos)) return true;
    if (keypadPanel->onMouseDown(mousePos)) return true;
    if (authCodePanel->onMouseDown(mousePos)) return true;
    return false;
}

bool ControlPanel::onMouseUp(Vector2 mousePos) {
    // Delegate to sub-panels
    if (launchSequencePanel->onMouseUp(mousePos)) return true;
    if (keypadPanel->onMouseUp(mousePos)) return true;
    if (authCodePanel->onMouseUp(mousePos)) return true;
    return false;
}

bool ControlPanel::onMouseMove(Vector2 mousePos) {
    // Delegate to sub-panels
    if (launchSequencePanel->onMouseMove(mousePos)) return true;
    if (keypadPanel->onMouseMove(mousePos)) return true;
    if (authCodePanel->onMouseMove(mousePos)) return true;
    return false;
}

void ControlPanel::handleKeypadInput(char key) {
    authCodePanel->handleKeypadInput(key);
}

void ControlPanel::handleBackspace() {
    authCodePanel->handleBackspace();
}

void ControlPanel::onAuthorize() {
    // This is now handled by the LaunchSequencePanel directly
}
