#include "AuthCodePanel.h"
#include <cctype>

AuthCodePanel::AuthCodePanel(AuthCodeCallback onAuthCodeSubmit)
    : authCodeCallback(onAuthCodeSubmit), authCodeEntered(false) {
    
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
    
    // Set initial bounds and layout
    setBounds(Rectangle{0, 0, 200, 100});
}

void AuthCodePanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void AuthCodePanel::setupLayout() {
    // Calculate layout for horizontal input boxes
    float margin = 15;
    float inputHeight = 30;
    float inputWidth = (bounds.width - 3 * margin) / 2;
    
    // Auth area
    authArea.x = bounds.x + margin;
    authArea.y = bounds.y + margin;
    authArea.width = bounds.width - 2 * margin;
    authArea.height = inputHeight;
    
    // Position auth input boxes horizontally
    float authY = authArea.y;
    authCodeInput->setBounds(Rectangle{authArea.x, authY, inputWidth, inputHeight});
    authCodeDisplay->setBounds(Rectangle{authArea.x + inputWidth + margin, authY, inputWidth, inputHeight});
}

void AuthCodePanel::setAuthCode(const std::string& code) {
    currentAuthCode = code;
    authCodeDisplay->setValue(code);
}

void AuthCodePanel::clearInput() {
    authCodeInput->clear();
}

void AuthCodePanel::setInputBorderColor(Color color) {
    authCodeInput->setBorderColor(color);
}

bool AuthCodePanel::isInputComplete() const {
    return authCodeInput->isComplete();
}

std::string AuthCodePanel::getInputValue() const {
    return authCodeInput->getValue();
}

void AuthCodePanel::handleAuthCodeSubmit() {
    currentAuthCode = authCodeInput->getValue();
    // Removed validation logic - just call callback
    if (authCodeCallback) {
        authCodeCallback(currentAuthCode);
    }
}

void AuthCodePanel::update(float dt) {
    // Check if input is complete and auto-submit
    if (authCodeInput->isComplete()) {
        handleAuthCodeSubmit();
    }
}

void AuthCodePanel::draw() const {
    // Draw authentication section
    DrawText("Enter code:", authArea.x, authArea.y - 25, 16, {0, 255, 255, 255});
    authCodeInput->draw();
    
    DrawText("AUTH CODE:", authArea.x + authArea.width / 2, authArea.y - 25, 16, {0, 255, 255, 255});
    authCodeDisplay->draw();
}

bool AuthCodePanel::onMouseDown(Vector2 mousePos) {
    // Input boxes don't handle mouse events directly
    return false;
}

bool AuthCodePanel::onMouseUp(Vector2 mousePos) {
    // Input boxes don't handle mouse events directly
    return false;
}

bool AuthCodePanel::onMouseMove(Vector2 mousePos) {
    // Input boxes don't handle mouse events directly
    return false;
}

void AuthCodePanel::handleKeypadInput(char key) {
    if (isdigit(key)) {
        authCodeInput->addDigit(key);
        
        // Auto-submit when 4 digits are entered
        if (authCodeInput->isComplete()) {
            handleAuthCodeSubmit();
        }
    }
}

void AuthCodePanel::handleBackspace() {
    authCodeInput->removeDigit();
}
