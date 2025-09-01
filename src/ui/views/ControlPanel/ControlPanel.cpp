#include "ControlPanel.h"
#include "../../../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"

ControlPanel::ControlPanel(SimulationEngine& engine, LaunchSequenceHandler* launchSequence) 
    : sequenceHandler(launchSequence) {
    
    // sub panels
    launchSequencePanel = std::make_unique<LaunchSequencePanel>(sequenceHandler);
    keypadPanel = std::make_unique<KeypadPanel>(
        [this](char key) { authCodePanel->handleKeypadInput(key); },
        [this]() { authCodePanel->handleBackspace(); }
    );
    authCodePanel = std::make_unique<AuthCodePanel>(
        [this](const std::string& code) { handleAuthCodeSubmit(code); }
    );
    
    // phase display
    phaseDisplay = std::make_unique<LaunchPhaseDisplay>();
    
    setBounds(Rectangle{0, 0, 400, 600});
}

void ControlPanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void ControlPanel::setupLayout() {
    float margin = 15;
    float leftWidth = bounds.width * 0.35f;  // launch buttons
    float rightWidth = bounds.width * 0.35f; // auth and keypad
    float centerGap = bounds.width * 0.20f;  // visual separation
    
    // left side: launch controls
    leftPanelArea.x = bounds.x + margin;
    leftPanelArea.y = bounds.y + margin;
    leftPanelArea.width = leftWidth;
    leftPanelArea.height = bounds.height - 2 * margin;
    
    // righ side: auth controls
    rightPanelArea.x = bounds.x + leftWidth + centerGap + margin;
    rightPanelArea.y = bounds.y + margin;
    rightPanelArea.width = rightWidth;
    rightPanelArea.height = bounds.height - 2 * margin;
    
    launchSequencePanel->setBounds(leftPanelArea);
    
    // phase display at top left
    Rectangle phaseDisplayArea = leftPanelArea;
    phaseDisplayArea.y = leftPanelArea.y + 50;
    phaseDisplayArea.height = 50;
    phaseDisplay->setBounds(phaseDisplayArea);
    
    // divide right side
    Rectangle authArea = rightPanelArea;
    authArea.height = 100;
    
    Rectangle keypadArea = rightPanelArea;
    keypadArea.y = authArea.y + authArea.height + margin - 20;
    keypadArea.height = 160;
    keypadArea.width = 120;
    keypadArea.x = rightPanelArea.x + (rightPanelArea.width - keypadArea.width) / 2;
    
    authCodePanel->setBounds(authArea);
    keypadPanel->setBounds(keypadArea);
}

void ControlPanel::handleAuthCodeSubmit(const std::string& code) {
    if (sequenceHandler) {
        sequenceHandler->submitAuthorization(code);
    }
}

void ControlPanel::update(float dt) {
    // display current launch phase
    if (sequenceHandler) {
        phaseDisplay->setCurrentPhase(sequenceHandler->getCurrentPhaseString());
        
        // show current auth code
        const std::string& currentAuthCode = sequenceHandler->getAuthCode();
        if (!currentAuthCode.empty()) {
            authCodePanel->setAuthCode(currentAuthCode);
        } else {
            authCodePanel->clearAuthCodeDisplay();
            authCodePanel->clearInput();
        }
    }
    
    launchSequencePanel->update(dt);
    keypadPanel->update(dt);
    authCodePanel->update(dt);
}

void ControlPanel::draw() const {
    DrawRectangleRec(bounds, {15, 15, 15, 255});
    
    DrawText("CONTROL PANEL", bounds.x + 20, bounds.y + 10, 20, RAYWHITE);

    launchSequencePanel->draw();
    keypadPanel->draw();
    authCodePanel->draw();
    phaseDisplay->draw();
}

bool ControlPanel::onMouseDown(Vector2 mousePos) {
    // pass to sub panels
    if (launchSequencePanel->onMouseDown(mousePos)) return true;
    if (keypadPanel->onMouseDown(mousePos)) return true;
    if (authCodePanel->onMouseDown(mousePos)) return true;
    return false;
}

bool ControlPanel::onMouseUp(Vector2 mousePos) {
    // pass to sub panels
    if (launchSequencePanel->onMouseUp(mousePos)) return true;
    if (keypadPanel->onMouseUp(mousePos)) return true;
    if (authCodePanel->onMouseUp(mousePos)) return true;
    return false;
}

bool ControlPanel::onMouseMove(Vector2 mousePos) {
    // pass to sub panels
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
    // leftover function i don't feel like deleting. lol
}
