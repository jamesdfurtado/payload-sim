#pragma once

#include "../Widget.h"
#include <string>
#include <raylib.h>

class TextBox : public Widget {
public:
    enum class Mode {
        DISPLAY,  // read-only display
        INPUT,    // editable input
    };

    struct TextBoxStyle {
        Color backgroundColor = {20, 20, 20, 255};
        Color borderColor = GRAY;
        Color textColor = WHITE;
        Color placeholderColor = {100, 100, 100, 255};
        int borderThickness = 2;
        int textSize = 20;
        int maxLength = 4;
        bool showBorder = true;
        bool showPlaceholder = true;
        std::string placeholder = "_";
    };

    explicit TextBox(Mode mode = Mode::DISPLAY, int maxLength = 4)
        : mode(mode), maxLength(maxLength), currentText(""), style() {
        setupDefaultStyle();
    }

    void setMode(Mode newMode) { mode = newMode; }
    Mode getMode() const { return mode; }

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

    // keypad input
    void addCharacter(char ch) {
        if (mode == Mode::INPUT && currentText.length() < maxLength) {
            if (isdigit(ch)) {
                currentText += ch;
            }
        }
    }

    void removeCharacter() {
        if (mode == Mode::INPUT && !currentText.empty()) {
            currentText.pop_back();
        }
    }

    void setStyle(const TextBoxStyle& newStyle) { style = newStyle; }
    void setBorderColor(Color color) { style.borderColor = color; }
    void setTextColor(Color color) { style.textColor = color; }
    void setBackgroundColor(Color color) { style.backgroundColor = color; }
    void setTextSize(int size) { style.textSize = size; }
    void setBorderThickness(int thickness) { style.borderThickness = thickness; }
    void setMaxLength(int length) { maxLength = length; }

    // standard widget methods
    void update(float dt) override {

    }

    void draw() const override {
        DrawRectangleRec(bounds, style.backgroundColor);
        
        if (style.showBorder) {
            DrawRectangleLinesEx(bounds, style.borderThickness, style.borderColor);
        }

        int textX = (int)(bounds.x + 10);
        int textY = (int)(bounds.y + (bounds.height - style.textSize) / 2);

        if (currentText.empty() && style.showPlaceholder) {
            for (int i = 0; i < maxLength; i++) {
                int placeholderX = textX + i * (style.textSize * 0.6f);
                DrawText(style.placeholder.c_str(), placeholderX, textY, style.textSize, style.placeholderColor);
            }
        } else {
            DrawText(currentText.c_str(), textX, textY, style.textSize, style.textColor);
        }
    }

    // mouse handling
    bool onMouseDown(Vector2 pos) override {
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
