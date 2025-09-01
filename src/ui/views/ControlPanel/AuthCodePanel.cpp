#include "AuthCodePanel.h"
#include <cctype>

AuthCodePanel::AuthCodePanel(AuthCodeCallback onAuthCodeSubmit)
    : authCodeCallback(onAuthCodeSubmit), authCodeEntered(false) {
    
    // input and display boxes
    inputBox = std::make_unique<TextBox>(TextBox::Mode::INPUT, 4);
    displayBox = std::make_unique<TextBox>(TextBox::Mode::DISPLAY, 4);
    
    // dark input styling
    TextBox::TextBoxStyle inputStyle;
    inputStyle.backgroundColor = {20, 20, 20, 255};
    inputStyle.borderColor = GRAY;
    inputStyle.textColor = WHITE;
    inputStyle.textSize = 20;
    inputBox->setStyle(inputStyle);
    
    // auth code display styling
    TextBox::TextBoxStyle displayStyle;
    displayStyle.backgroundColor = {20, 20, 20, 255};
    displayStyle.borderColor = GRAY;
    displayStyle.textColor = YELLOW;
    displayStyle.textSize = 20;
    displayBox->setStyle(displayStyle);
    
    displayBox->setText("----");
    
    setBounds(Rectangle{0, 0, 200, 100});
}

void AuthCodePanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void AuthCodePanel::setupLayout() {
    float margin = 15;
    float inputHeight = 30;
    float boxWidth = (bounds.width - 3 * margin) / 2;
    
    authArea.x = bounds.x + margin;
    authArea.y = bounds.y + margin;
    authArea.width = bounds.width - 2 * margin;
    authArea.height = inputHeight;
    
    inputBox->setBounds(Rectangle{authArea.x, authArea.y, boxWidth, authArea.height});
    displayBox->setBounds(Rectangle{authArea.x + boxWidth + margin + 20, authArea.y, boxWidth, authArea.height});
}

void AuthCodePanel::setAuthCode(const std::string& code) {
    currentAuthCode = code;
    if (!code.empty()) {
        displayBox->setText(code);
    } else {
        displayBox->setText("----");
    }
}

void AuthCodePanel::clearInput() {
    inputBox->clear();
}

void AuthCodePanel::clearAuthCodeDisplay() {
    currentAuthCode.clear();
    displayBox->setText("----");
}

void AuthCodePanel::setInputBorderColor(Color color) {
    inputBox->setBorderColor(color);
}

bool AuthCodePanel::isInputComplete() const {
    return inputBox->isComplete();
}

std::string AuthCodePanel::getInputValue() const {
    return inputBox->getText();
}

void AuthCodePanel::handleAuthCodeSubmit() {
    currentAuthCode = inputBox->getText();
    if (authCodeCallback) {
        authCodeCallback(currentAuthCode);
    }
}

void AuthCodePanel::update(float dt) {
    // auto-submit when full
    if (isInputComplete()) {
        handleAuthCodeSubmit();
    }
}

void AuthCodePanel::draw() const {
    DrawText("Enter code:", authArea.x, authArea.y - 25, 16, {0, 255, 255, 255});
    DrawText("AUTH CODE:", authArea.x + authArea.width / 2 + 20, authArea.y - 25, 16, {0, 255, 255, 255});
    
    inputBox->draw();
    displayBox->draw();
}

bool AuthCodePanel::onMouseDown(Vector2 mousePos) {
    // pass to textboxes
    if (inputBox->onMouseDown(mousePos)) return true;
    if (displayBox->onMouseDown(mousePos)) return true;
    return false;
}

bool AuthCodePanel::onMouseUp(Vector2 mousePos) {
    // pass to textboxes
    if (inputBox->onMouseUp(mousePos)) return true;
    if (displayBox->onMouseUp(mousePos)) return true;
    return false;
}

bool AuthCodePanel::onMouseMove(Vector2 mousePos) {
    // pass to textboxes
    if (inputBox->onMouseMove(mousePos)) return true;
    if (displayBox->onMouseMove(mousePos)) return true;
    return false;
}

void AuthCodePanel::handleKeypadInput(char key) {
    if (isdigit(key)) {
        inputBox->addCharacter(key);
        
        // submit when complete
        if (isInputComplete()) {
            handleAuthCodeSubmit();
        }
    }
}

void AuthCodePanel::handleBackspace() {
    inputBox->removeCharacter();
}
