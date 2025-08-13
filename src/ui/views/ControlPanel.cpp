#include "ControlPanel.h"
#include <algorithm>
#include <cctype>

ControlPanel::ControlPanel(SafetySystem* safetySystem) 
    : safetySystem(safetySystem), authCodeEntered(false) {
    
    // Create state buttons with appropriate colors
    authorizeButton = std::make_unique<Button>("AUTHORIZE LAUNCH", [this]() { onAuthorize(); });
    authorizeButton->setColors(BLUE, BLUE, WHITE);
    
    armButton = std::make_unique<Button>("ARM WEAPON", [this]() { onArm(); });
    armButton->setColors(PURPLE, PURPLE, WHITE);
    
    launchButton = std::make_unique<Button>("LAUNCH PAYLOAD", [this]() { onLaunch(); });
    launchButton->setColors(GREEN, GREEN, WHITE);
    
    resetButton = std::make_unique<Button>("RESET SYSTEM", [this]() { onReset(); });
    resetButton->setColors(RED, RED, WHITE);
    
    // Create authentication input boxes
    authCodeInput = std::make_unique<InputBox>(4);
    InputBox::InputBoxStyle inputStyle;
    inputStyle.backgroundColor = {20, 20, 20, 255};
    inputStyle.borderColor = GREEN;
    inputStyle.textColor = WHITE;
    inputStyle.textSize = 20;
    authCodeInput->setStyle(inputStyle);
    
    authCodeDisplay = std::make_unique<InputBox>(4);
    InputBox::InputBoxStyle displayStyle;
    displayStyle.backgroundColor = {20, 20, 20, 255};
    displayStyle.borderColor = {0, 255, 255, 255}; // Cyan
    displayStyle.textColor = YELLOW;
    displayStyle.textSize = 20;
    authCodeDisplay->setStyle(displayStyle);
    
    // Set initial auth code display
    authCodeDisplay->setValue("1511");
    
    // Create keypad
    createKeypad();
    
    // Set initial bounds and layout
    setBounds(Rectangle{0, 0, 400, 600});
}

void ControlPanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void ControlPanel::setupLayout() {
    // Calculate layout areas for horizontal layout with more separation
    float margin = 15;
    float buttonHeight = 35;
    float leftWidth = bounds.width * 0.35f;  // Left side for state buttons (smaller)
    float rightWidth = bounds.width * 0.35f; // Right side for auth and keypad (smaller)
    float centerGap = bounds.width * 0.20f;  // Center gap for separation
    
    // State display area (top, spans full width)
    stateDisplayArea.x = bounds.x + margin;
    stateDisplayArea.y = bounds.y + margin;
    stateDisplayArea.width = bounds.width - 2 * margin;
    stateDisplayArea.height = 40;
    
    // Left side: Button area (below state display)
    buttonArea.x = bounds.x + margin;
    buttonArea.y = stateDisplayArea.y + stateDisplayArea.height + margin;
    buttonArea.width = leftWidth;
    buttonArea.height = 4 * buttonHeight + 3 * margin;
    
    // Right side: Authentication area (below state display)
    authArea.x = bounds.x + leftWidth + centerGap + margin;
    authArea.y = stateDisplayArea.y + stateDisplayArea.height + margin + 10; // Move down 10px
    authArea.width = rightWidth;
    authArea.height = 80;
    
    // Right side: Keypad area (below auth)
    keypadArea.x = bounds.x + leftWidth + centerGap + margin;
    keypadArea.y = authArea.y + authArea.height + margin - 10; // Move up 10px
    keypadArea.width = (rightWidth * 0.8f) * 0.5f; // Make keypad half size (50% of 80% of right width)
    keypadArea.height = (bounds.height - (authArea.y + authArea.height + margin) - margin) * 0.5f; // Half height too
    
    // Position state buttons (left side, vertical stack)
    float buttonY = buttonArea.y;
    authorizeButton->setBounds(Rectangle{buttonArea.x, buttonY, leftWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    armButton->setBounds(Rectangle{buttonArea.x, buttonY, leftWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    launchButton->setBounds(Rectangle{buttonArea.x, buttonY, leftWidth, buttonHeight});
    buttonY += buttonHeight + margin;
    
    resetButton->setBounds(Rectangle{buttonArea.x, buttonY, leftWidth, buttonHeight});
    
    // Position auth input boxes (right side, horizontal)
    float authY = authArea.y;
    authCodeInput->setBounds(Rectangle{authArea.x, authY, rightWidth / 2 - margin / 2, 30});
    authCodeDisplay->setBounds(Rectangle{authArea.x + rightWidth / 2 + margin / 2, authY, rightWidth / 2 - margin / 2, 30});
    
    // Position keypad buttons (3x3 grid, right side, centered)
    float keypadButtonSize = (keypadArea.width - 2 * margin) / 3;
    float keypadX = keypadArea.x + (keypadArea.width - (3 * keypadButtonSize + 2 * margin)) / 2; // Center horizontally
    float keypadY = keypadArea.y;
    
    for (size_t i = 0; i < keypadButtons.size(); ++i) {
        if (i % 3 == 0 && i > 0) {
            keypadX = keypadArea.x;
            keypadY += keypadButtonSize + margin;
        }
        
        // Special positioning for backspace button (bottom right)
        if (i == 10) { // Backspace button
            keypadButtons[i]->setBounds(Rectangle{
                keypadArea.x + 2 * (keypadButtonSize + margin),
                keypadY,
                keypadButtonSize,
                keypadButtonSize
            });
        } else {
            keypadButtons[i]->setBounds(Rectangle{
                keypadX,
                keypadY,
                keypadButtonSize,
                keypadButtonSize
            });
            keypadX += keypadButtonSize + margin;
        }
    }
}

void ControlPanel::createKeypad() {
    // Create numeric keypad buttons (1-9, 0, backspace)
    std::vector<char> keys = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '0'};
    
    for (char key : keys) {
        // Number button
        auto numBtn = std::make_unique<Button>(key, [this](char k) { 
            handleKeypadInput(k); 
        });
        numBtn->setColors(DARKGRAY, WHITE, WHITE);
        keypadButtons.push_back(std::move(numBtn));
    }
    
    // Create backspace button separately
    auto backspaceBtn = std::make_unique<Button>("←", [this]() { 
        authCodeInput->removeDigit(); 
    });
    backspaceBtn->setColors(RED, RED, WHITE);
    backspaceBtn->setTextSize(16);
    keypadButtons.push_back(std::move(backspaceBtn));
}

void ControlPanel::handleKeypadInput(char key) {
    if (isdigit(key)) {
        authCodeInput->addDigit(key);
        
        // Auto-submit when 4 digits are entered
        if (authCodeInput->isComplete()) {
            handleAuthCodeSubmit();
        }
    }
}

void ControlPanel::handleAuthCodeSubmit() {
    currentAuthCode = authCodeInput->getValue();
    if (currentAuthCode == "1511") {
        authCodeEntered = true;
        authCodeInput->setBorderColor(GREEN);
        // Submit to safety system
        safetySystem->requestAuthorizationCode(currentAuthCode.c_str());
    } else {
        authCodeInput->setBorderColor(RED);
        authCodeInput->clear();
    }
}

void ControlPanel::updateButtonStates() {
    LaunchPhase currentPhase = safetySystem->getPhase();
    
    // Update button states based on current phase
    switch (currentPhase) {
        case LaunchPhase::Idle:
            authorizeButton->setEnabled(true);
            armButton->setEnabled(false);
            launchButton->setEnabled(false);
            resetButton->setEnabled(false);
            break;
            
        case LaunchPhase::Authorized:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(true);
            launchButton->setEnabled(false);
            resetButton->setEnabled(true);
            break;
            
        case LaunchPhase::Arming:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(false);
            launchButton->setEnabled(false);
            resetButton->setEnabled(true);
            break;
            
        case LaunchPhase::Armed:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(false);
            launchButton->setEnabled(true);
            resetButton->setEnabled(true);
            break;
            
        case LaunchPhase::Launching:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(false);
            launchButton->setEnabled(false);
            resetButton->setEnabled(false);
            break;
            
        case LaunchPhase::Launched:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(false);
            launchButton->setEnabled(false);
            resetButton->setEnabled(true);
            break;
            
        case LaunchPhase::Resetting:
            authorizeButton->setEnabled(false);
            armButton->setEnabled(false);
            launchButton->setEnabled(false);
            resetButton->setEnabled(false);
            break;
    }
}

void ControlPanel::update(float dt) {
    updateButtonStates();
    
    // Update all buttons
    authorizeButton->update(dt);
    armButton->update(dt);
    launchButton->update(dt);
    resetButton->update(dt);
    
    // Update keypad buttons
    for (auto& button : keypadButtons) {
        button->update(dt);
    }
}

void ControlPanel::draw() const {
    // Draw background
    DrawRectangleRec(bounds, {15, 15, 15, 255});
    
    // Draw title
    DrawText("CONTROL PANEL", bounds.x + 20, bounds.y + 10, 20, RAYWHITE);
    
    // Draw payload state display
    LaunchPhase currentPhase = safetySystem->getPhase();
    std::string stateText = "Payload State: " + getPhaseString(currentPhase);
    Color stateColor = getPhaseColor(currentPhase);
    
    DrawRectangleRec(stateDisplayArea, {25, 25, 25, 255});
    DrawRectangleLinesEx(stateDisplayArea, 2, stateColor);
    
    int textWidth = MeasureText(stateText.c_str(), 18);
    int textX = (int)(stateDisplayArea.x + (stateDisplayArea.width - textWidth) / 2);
    int textY = (int)(stateDisplayArea.y + (stateDisplayArea.height - 18) / 2);
    DrawText(stateText.c_str(), textX, textY, 18, stateColor);
    
    // Draw state buttons
    authorizeButton->draw();
    armButton->draw();
    launchButton->draw();
    resetButton->draw();
    
    // Draw authentication section
    DrawText("Enter code:", authArea.x, authArea.y - 25, 16, {0, 255, 255, 255});
    authCodeInput->draw();
    
    DrawText("AUTH CODE:", authArea.x + authArea.width / 2, authArea.y - 25, 16, {0, 255, 255, 255});
    authCodeDisplay->draw();
    
    // Draw keypad
    DrawText("KEYPAD", keypadArea.x, keypadArea.y - 25, 16, RAYWHITE);
    for (const auto& button : keypadButtons) {
        button->draw();
    }
}

bool ControlPanel::onMouseDown(Vector2 mousePos) {
    // Check state buttons
    if (authorizeButton->onMouseDown(mousePos)) return true;
    if (armButton->onMouseDown(mousePos)) return true;
    if (launchButton->onMouseDown(mousePos)) return true;
    if (resetButton->onMouseDown(mousePos)) return true;
    
    // Check keypad buttons
    for (auto& button : keypadButtons) {
        if (button->onMouseDown(mousePos)) return true;
    }
    
    return false;
}

bool ControlPanel::onMouseUp(Vector2 mousePos) {
    // Check state buttons
    if (authorizeButton->onMouseUp(mousePos)) return true;
    if (armButton->onMouseUp(mousePos)) return true;
    if (launchButton->onMouseUp(mousePos)) return true;
    if (resetButton->onMouseUp(mousePos)) return true;
    
    // Check keypad buttons
    for (auto& button : keypadButtons) {
        if (button->onMouseUp(mousePos)) return true;
    }
    
    return false;
}

bool ControlPanel::onMouseMove(Vector2 mousePos) {
    // Check state buttons
    if (authorizeButton->onMouseMove(mousePos)) return true;
    if (armButton->onMouseMove(mousePos)) return true;
    if (launchButton->onMouseMove(mousePos)) return true;
    if (resetButton->onMouseMove(mousePos)) return true;
    
    // Check keypad buttons
    for (auto& button : keypadButtons) {
        if (button->onMouseMove(mousePos)) return true;
    }
    
    return false;
}

void ControlPanel::onAuthorize() {
    if (authCodeEntered) {
        safetySystem->requestAuthorizationCode(currentAuthCode.c_str());
    }
}

void ControlPanel::onArm() {
    safetySystem->arm();
}

void ControlPanel::onLaunch() {
    safetySystem->launch();
}

void ControlPanel::onReset() {
    safetySystem->reset();
    authCodeEntered = false;
    authCodeInput->clear();
    authCodeInput->setBorderColor(GREEN);
}

std::string ControlPanel::getPhaseString(LaunchPhase phase) const {
    switch (phase) {
        case LaunchPhase::Idle: return "IDLE";
        case LaunchPhase::Authorized: return "AUTHORIZED";
        case LaunchPhase::Arming: return "ARMING";
        case LaunchPhase::Armed: return "ARMED";
        case LaunchPhase::Launching: return "LAUNCHING";
        case LaunchPhase::Launched: return "LAUNCHED";
        case LaunchPhase::Resetting: return "RESETTING";
        default: return "UNKNOWN";
    }
}

Color ControlPanel::getPhaseColor(LaunchPhase phase) const {
    switch (phase) {
        case LaunchPhase::Idle: return GRAY;
        case LaunchPhase::Authorized: return BLUE;
        case LaunchPhase::Arming: return YELLOW;
        case LaunchPhase::Armed: return PURPLE;
        case LaunchPhase::Launching: return ORANGE;
        case LaunchPhase::Launched: return GREEN;
        case LaunchPhase::Resetting: return RED;
        default: return WHITE;
    }
}
