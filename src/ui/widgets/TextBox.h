#pragma once

#include "../Widget.h"
#include <string>
#include <raylib.h>

class TextBox : public Widget {
public:
    enum class Mode {
        INPUT,      // Can receive input and handle keypad events
        DISPLAY     // Read-only display of text
    };

    struct TextBoxStyle {
        Color backgroundColor = {20, 20, 20, 255};  // Dark background
        Color borderColor = GRAY;                    // Border color
        Color textColor = WHITE;                     // Text color
        Color placeholderColor = {100, 100, 100, 255}; // Placeholder color
        int borderThickness = 2;
        int textSize = 20;
        int maxLength = 4;                           // Maximum characters
        bool showBorder = true;
        bool showPlaceholder = true;
        std::string placeholder = "_";               // Placeholder character
    };

    explicit TextBox(Mode mode = Mode::DISPLAY, int maxLength = 4)
        : mode(mode), maxLength(maxLength), currentText(""), style() {
        setupDefaultStyle();
    }

    // Mode management
    void setMode(Mode newMode) { mode = newMode; }
    Mode getMode() const { return mode; }

    // Text management
    void setText(const std::string& text) {
        if (text.length() <= maxLength) {
            currentText = text;
        }
    }

    void clear() { currentText.clear(); }
    const std::string& getText() const { return currentText; }
    bool isEmpty() const { return currentText.empty(); }
    bool isComplete() const { return currentText.length() == maxLength; }
    int getCurrentLength() const { return currentText.length(); }

    // Input handling (only for INPUT mode)
    void addCharacter(char ch) {
        if (mode == Mode::INPUT && currentText.length() < maxLength) {
            if (isdigit(ch)) {  // Only allow digits for now
                currentText += ch;
            }
        }
    }

    void removeCharacter() {
        if (mode == Mode::INPUT && !currentText.empty()) {
            currentText.pop_back();
        }
    }

    // Style customization
    void setStyle(const TextBoxStyle& newStyle) { style = newStyle; }
    void setBorderColor(Color color) { style.borderColor = color; }
    void setTextColor(Color color) { style.textColor = color; }
    void setBackgroundColor(Color color) { style.backgroundColor = color; }
    void setTextSize(int size) { style.textSize = size; }
    void setBorderThickness(int thickness) { style.borderThickness = thickness; }
    void setMaxLength(int length) { maxLength = length; }

    // Widget interface
    void update(float dt) override {
        // TextBox state updates can go here if needed
    }

    void draw() const override {
        // Draw background
        DrawRectangleRec(bounds, style.backgroundColor);
        
        // Draw border if enabled
        if (style.showBorder) {
            DrawRectangleLinesEx(bounds, style.borderThickness, style.borderColor);
        }

        // Calculate text positioning
        int textX = (int)(bounds.x + 10);
        int textY = (int)(bounds.y + (bounds.height - style.textSize) / 2);

        if (currentText.empty() && style.showPlaceholder) {
            // Draw placeholder characters
            for (int i = 0; i < maxLength; i++) {
                int placeholderX = textX + i * (style.textSize * 0.6f);
                DrawText(style.placeholder.c_str(), placeholderX, textY, style.textSize, style.placeholderColor);
            }
        } else {
            // Draw actual text
            DrawText(currentText.c_str(), textX, textY, style.textSize, style.textColor);
        }
    }

    // Mouse interaction (could implement click-to-focus for INPUT mode)
    bool onMouseDown(Vector2 pos) override {
        // Could implement click-to-focus in the future
        return false;
    }

    bool onMouseUp(Vector2 pos) override {
        return false;
    }

    bool onMouseMove(Vector2 pos) override {
        return false;
    }

private:
    void setupDefaultStyle() {
        style = TextBoxStyle();
    }

    Mode mode;
    int maxLength;
    std::string currentText;
    TextBoxStyle style;
};
