#include "KeypadPanel.h"

KeypadPanel::KeypadPanel(KeypadCallback onKeyPress, BackspaceCallback onBackspace)
    : keypadCallback(onKeyPress), backspaceCallback(onBackspace) {
    
    // Create keypad
    createKeypad();
    
    // Set initial bounds and layout (smaller size)
    setBounds(Rectangle{0, 0, 120, 160});
}

void KeypadPanel::setBounds(Rectangle newBounds) {
    Widget::setBounds(newBounds);
    setupLayout();
}

void KeypadPanel::setupLayout() {
    // Calculate layout for 3x3 grid with backspace
    float margin = 10;
    float buttonSize = (bounds.width - 2 * margin) / 3;
    
    // Keypad area
    keypadArea.x = bounds.x + margin;
    keypadArea.y = bounds.y + margin;
    keypadArea.width = bounds.width - 2 * margin;
    keypadArea.height = bounds.height - 2 * margin;
    
    // Position keypad buttons (3x3 grid + backspace)
    float keypadX = keypadArea.x;
    float keypadY = keypadArea.y;
    
    for (size_t i = 0; i < keypadButtons.size(); ++i) {
        if (i % 3 == 0 && i > 0) {
            keypadX = keypadArea.x;
            keypadY += buttonSize + margin;
        }
        
        // Special positioning for backspace button (bottom right)
        if (i == 10) { // Backspace button
            keypadButtons[i]->setBounds(Rectangle{
                keypadArea.x + 2 * (buttonSize + margin),
                keypadY,
                buttonSize,
                buttonSize
            });
        } else {
            keypadButtons[i]->setBounds(Rectangle{
                keypadX,
                keypadY,
                buttonSize,
                buttonSize
            });
            keypadX += buttonSize + margin;
        }
    }
}

void KeypadPanel::createKeypad() {
    // Create numeric keypad buttons (1-9, 0, backspace)
    std::vector<char> keys = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '0'};
    
    for (char key : keys) {
        // Number button
        auto numBtn = std::make_unique<Button>(key, [this, key](char pressedKey) { 
            if (keypadCallback) {
                keypadCallback(pressedKey); 
            }
        });
        numBtn->setColors(DARKGRAY, WHITE, WHITE);
        keypadButtons.push_back(std::move(numBtn));
    }
    
    // Create backspace button separately
    auto backspaceBtn = std::make_unique<Button>("<", [this]() { 
        if (backspaceCallback) {
            backspaceCallback(); 
        }
    });
    backspaceBtn->setColors(RED, RED, WHITE);
    backspaceBtn->setTextSize(16);
    keypadButtons.push_back(std::move(backspaceBtn));
}

void KeypadPanel::update(float dt) {
    // Update all keypad buttons
    for (auto& button : keypadButtons) {
        button->update(dt);
    }
}

void KeypadPanel::draw() const {
    // Draw title (moved up a bit)
    DrawText("KEYPAD", bounds.x + 10, bounds.y - 15, 16, RAYWHITE);
    
    // Draw keypad buttons
    for (const auto& button : keypadButtons) {
        button->draw();
    }
}

bool KeypadPanel::onMouseDown(Vector2 mousePos) {
    for (auto& button : keypadButtons) {
        if (button->onMouseDown(mousePos)) return true;
    }
    return false;
}

bool KeypadPanel::onMouseUp(Vector2 mousePos) {
    for (auto& button : keypadButtons) {
        if (button->onMouseUp(mousePos)) return true;
    }
    return false;
}

bool KeypadPanel::onMouseMove(Vector2 mousePos) {
    for (auto& button : keypadButtons) {
        if (button->onMouseMove(mousePos)) return true;
    }
    return false;
}
