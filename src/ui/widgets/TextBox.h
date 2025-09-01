#pragma once

#include "../Widget.h"
#include <string>
#include <raylib.h>

class TextBox : public Widget {
public:
    enum class Mode {
        INPUT,      // accepts keypad input
        DISPLAY     // read-only display
    };

    struct TextBoxStyle {
        Color backgroundColor = {20, 20, 20, 255};  // dark background
        Color borderColor = GRAY;                    // border color
        Color textColor = WHITE;                     // text color
        Color placeholderColor = {100, 100, 100, 255}; // placeholder color
        int borderThickness = 2;
        int textSize = 20;
        int maxLength = 4;                           // character limit
        bool showBorder = true;
        bool showPlaceholder = true;
        std::string placeholder = "_";               // empty slot character
    };

    explicit TextBox(Mode mode = Mode::DISPLAY, int maxLength = 4)
        : mode(mode), maxLength(maxLength), currentText(""), style() {
        setupDefaultStyle();
    }

    // input vs display mode
    void setMode(Mode newMode) { mode = newMode; }
    Mode getMode() const { return mode; }

    // content control
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

    // keypad input (input mode only)
    void addCharacter(char ch) {
        if (mode == Mode::INPUT && currentText.length() < maxLength) {
            if (isdigit(ch)) {  // digits only
                currentText += ch;
            }
        }
    }

    void removeCharacter() {
        if (mode == Mode::INPUT && !currentText.empty()) {
            currentText.pop_back();
        }
    }

    // appearance settings
    void setStyle(const TextBoxStyle& newStyle) { style = newStyle; }
    void setBorderColor(Color color) { style.borderColor = color; }
    void setTextColor(Color color) { style.textColor = color; }
    void setBackgroundColor(Color color) { style.backgroundColor = color; }
    void setTextSize(int size) { style.textSize = size; }
    void setBorderThickness(int thickness) { style.borderThickness = thickness; }
    void setMaxLength(int length) { maxLength = length; }

    // standard widget methods
    void update(float dt) override {
        // placeholder for future logic
    }

    void draw() const override {
        // background fill
        DrawRectangleRec(bounds, style.backgroundColor);
        
        // optional border
        if (style.showBorder) {
            DrawRectangleLinesEx(bounds, style.borderThickness, style.borderColor);
        }

        // center text vertically
        int textX = (int)(bounds.x + 10);
        int textY = (int)(bounds.y + (bounds.height - style.textSize) / 2);

        if (currentText.empty() && style.showPlaceholder) {
            // show empty slots
            for (int i = 0; i < maxLength; i++) {
                int placeholderX = textX + i * (style.textSize * 0.6f);
                DrawText(style.placeholder.c_str(), placeholderX, textY, style.textSize, style.placeholderColor);
            }
        } else {
            // render content
            DrawText(currentText.c_str(), textX, textY, style.textSize, style.textColor);
        }
    }

    // mouse handling (could add focus later)
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
