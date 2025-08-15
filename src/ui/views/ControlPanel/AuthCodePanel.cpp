#include "AuthCodePanel.h"
#include <cctype>

AuthCodePanel::AuthCodePanel(AuthCodeCallback onAuthCodeSubmit)
    : authCodeCallback(onAuthCodeSubmit), authCodeEntered(false), inputValue(""), displayValue("----") {
    
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
    float boxWidth = (bounds.width - 3 * margin) / 2;  // Equal width for both boxes
    
    // Auth area - now just for positioning reference
    authArea.x = bounds.x + margin;
    authArea.y = bounds.y + margin;
    authArea.width = bounds.width - 2 * margin;
    authArea.height = inputHeight;
}

void AuthCodePanel::setAuthCode(const std::string& code) {
    currentAuthCode = code;
    if (!code.empty()) {
        displayValue = code;
    } else {
        displayValue = "----";
    }
}

void AuthCodePanel::clearInput() {
    inputValue.clear();
}

void AuthCodePanel::clearAuthCodeDisplay() {
    currentAuthCode.clear();
    displayValue = "----";
}

void AuthCodePanel::setInputBorderColor(Color color) {
    // No longer needed - simplified implementation
}

bool AuthCodePanel::isInputComplete() const {
    return inputValue.length() == 4;
}

std::string AuthCodePanel::getInputValue() const {
    return inputValue;
}

void AuthCodePanel::handleAuthCodeSubmit() {
    currentAuthCode = inputValue;
    // Removed validation logic - just call callback
    if (authCodeCallback) {
        authCodeCallback(currentAuthCode);
    }
}

void AuthCodePanel::update(float dt) {
    // Check if input is complete and auto-submit
    if (isInputComplete()) {
        handleAuthCodeSubmit();
    }
}

void AuthCodePanel::draw() const {
    // Calculate box dimensions
    float margin = 15;
    float boxWidth = (bounds.width - 3 * margin) / 2;
    
    // Draw authentication section
    DrawText("Enter code:", authArea.x, authArea.y - 25, 16, {0, 255, 255, 255});
    
    // Draw input box (same size as display box)
    Rectangle inputBox = {authArea.x, authArea.y, boxWidth, authArea.height};
    DrawRectangleRec(inputBox, {20, 20, 20, 255});
    DrawRectangleLinesEx(inputBox, 2, GRAY);
    DrawText(inputValue.c_str(), inputBox.x + 10, inputBox.y + 5, 20, WHITE);
    
    DrawText("AUTH CODE:", authArea.x + authArea.width / 2 + 20, authArea.y - 25, 16, {0, 255, 255, 255});
    
    // Draw display box (same size as input box, moved 20 pixels right)
    Rectangle displayBox = {authArea.x + boxWidth + margin + 20, authArea.y, boxWidth, authArea.height};
    DrawRectangleRec(displayBox, {20, 20, 20, 255});
    DrawRectangleLinesEx(displayBox, 2, GRAY);
    DrawText(displayValue.c_str(), displayBox.x + 10, displayBox.y + 5, 20, YELLOW);
}

bool AuthCodePanel::onMouseDown(Vector2 mousePos) {
    // No mouse handling needed for simple text display
    return false;
}

bool AuthCodePanel::onMouseUp(Vector2 mousePos) {
    // No mouse handling needed for simple text display
    return false;
}

bool AuthCodePanel::onMouseMove(Vector2 mousePos) {
    // No mouse handling needed for simple text display
    return false;
}

void AuthCodePanel::handleKeypadInput(char key) {
    if (isdigit(key) && inputValue.length() < 4) {
        inputValue += key;
        
        // Auto-submit when 4 digits are entered
        if (isInputComplete()) {
            handleAuthCodeSubmit();
        }
    }
}

void AuthCodePanel::handleBackspace() {
    if (!inputValue.empty()) {
        inputValue.pop_back();
    }
}
