#include "LaunchSequencePanel.h"

LaunchSequencePanel::LaunchSequencePanel(LaunchSequenceHandler* handler) 
    : sequenceHandler(handler) {
    
    // Create state buttons with appropriate colors
    authorizeButton = std::make_unique<Button>("AUTHORIZE LAUNCH", [this]() { onAuthorize(); });
    authorizeButton->setColors(BLUE, BLUE, WHITE);
    
    armButton = std::make_unique<Button>("ARM WEAPON", [this]() { onArm(); });
    armButton->setColors(PURPLE, PURPLE, WHITE);
    
    launchButton = std::make_unique<Button>("LAUNCH PAYLOAD", [this]() { onLaunch(); });
    launchButton->setColors(GREEN, GREEN, WHITE);
    
    resetButton = std::make_unique<Button>("RESET SYSTEM", [this]() { onReset(); });
    resetButton->setColors(RED, RED, WHITE);
    
    // Set initial bounds and layout
    setBounds(Rectangle{0, 0, 200, 200});
}

void LaunchSequencePanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void LaunchSequencePanel::setupLayout() {
    // Calculate layout for vertical button stack
    float margin = 15;
    float buttonHeight = 35;
    float buttonWidth = bounds.width - 2 * margin;
    
    // Button area
    buttonArea.x = bounds.x + margin;
    buttonArea.y = bounds.y + margin;
    buttonArea.width = buttonWidth;
    buttonArea.height = 4 * buttonHeight + 3 * margin;
    
    // Position buttons vertically (moved down a bit)
    float buttonY = buttonArea.y + 80; // Add 80px offset to move buttons down
    authorizeButton->setBounds(Rectangle{buttonArea.x, buttonY, buttonWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    armButton->setBounds(Rectangle{buttonArea.x, buttonY, buttonWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    launchButton->setBounds(Rectangle{buttonArea.x, buttonY, buttonWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    resetButton->setBounds(Rectangle{buttonArea.x, buttonY, buttonWidth, buttonHeight});
}

void LaunchSequencePanel::update(float dt) {
    // Update all buttons
    authorizeButton->update(dt);
    armButton->update(dt);
    launchButton->update(dt);
    resetButton->update(dt);
}

void LaunchSequencePanel::draw() const {
    // Draw state buttons
    authorizeButton->draw();
    armButton->draw();
    launchButton->draw();
    resetButton->draw();
}

bool LaunchSequencePanel::onMouseDown(Vector2 mousePos) {
    if (authorizeButton->onMouseDown(mousePos)) return true;
    if (armButton->onMouseDown(mousePos)) return true;
    if (launchButton->onMouseDown(mousePos)) return true;
    if (resetButton->onMouseDown(mousePos)) return true;
    return false;
}

bool LaunchSequencePanel::onMouseUp(Vector2 mousePos) {
    if (authorizeButton->onMouseUp(mousePos)) return true;
    if (armButton->onMouseUp(mousePos)) return true;
    if (launchButton->onMouseUp(mousePos)) return true;
    if (resetButton->onMouseUp(mousePos)) return true;
    return false;
}

bool LaunchSequencePanel::onMouseMove(Vector2 mousePos) {
    if (authorizeButton->onMouseMove(mousePos)) return true;
    if (armButton->onMouseMove(mousePos)) return true;
    if (launchButton->onMouseMove(mousePos)) return true;
    if (resetButton->onMouseMove(mousePos)) return true;
    return false;
}

void LaunchSequencePanel::onAuthorize() {
    if (sequenceHandler) {
        sequenceHandler->requestAuthorization();
    }
}

void LaunchSequencePanel::onArm() {
    if (sequenceHandler) {
        sequenceHandler->requestArm();
    }
}

void LaunchSequencePanel::onLaunch() {
    if (sequenceHandler) {
        sequenceHandler->requestLaunch();
    }
}

void LaunchSequencePanel::onReset() {
    if (sequenceHandler) {
        sequenceHandler->requestReset();
    }
}