#pragma once

#include "../Widget.h"
#include <string>
#include <raylib.h>

class InputBox : public Widget {
public:
    struct InputBoxStyle {
        Color backgroundColor = Color{20, 20, 20, 255};  // Dark background
        Color borderColor = GREEN;                       // Green glowing border
        Color textColor = Color{0, 255, 255, 255};      // Cyan text
        Color placeholderColor = GRAY;                   // Gray for empty slots
        int borderThickness = 3;
        int textSize = 24;
        int digitSpacing = 8;                           // Space between digits
        bool showBorder = true;
        bool showPlaceholder = true;
    };

    explicit InputBox(int maxDigits = 4)
        : maxDigits(maxDigits), currentInput(""), style() {
        setupDefaultStyle();
    }

    // Input management
    void addDigit(char digit) {
        if (currentInput.length() < maxDigits && isdigit(digit)) {
            currentInput += digit;
        }
    }

    void removeDigit() {
        if (!currentInput.empty()) {
            currentInput.pop_back();
        }
    }

    void clear() {
        currentInput.clear();
    }

    void setValue(const std::string& value) {
        if (value.length() <= maxDigits) {
            currentInput = value;
        }
    }

    // Getters
    const std::string& getValue() const { return currentInput; }
    bool isComplete() const { return currentInput.length() == maxDigits; }
    int getMaxDigits() const { return maxDigits; }
    int getCurrentLength() const { return currentInput.length(); }

    // Style customization
    void setStyle(const InputBoxStyle& newStyle) { style = newStyle; }
    void setBorderColor(Color color) { style.borderColor = color; }
    void setTextColor(Color color) { style.textColor = color; }
    void setBackgroundColor(Color color) { style.backgroundColor = color; }
    void setTextSize(int size) { style.textSize = size; }
    void setBorderThickness(int thickness) { style.borderThickness = thickness; }
    void setDigitSpacing(int spacing) { style.digitSpacing = spacing; }

    void draw() const override {
        // Draw background
        DrawRectangleRec(bounds, style.backgroundColor);
        
        // Draw border if enabled
        if (style.showBorder) {
            DrawRectangleLinesEx(bounds, style.borderThickness, style.borderColor);
        }

        // Calculate digit positions
        float digitWidth = (bounds.width - (maxDigits - 1) * style.digitSpacing) / maxDigits;
        float startX = bounds.x + (bounds.width - (maxDigits * digitWidth + (maxDigits - 1) * style.digitSpacing)) / 2;

        // Draw digits or placeholders
        for (int i = 0; i < maxDigits; i++) {
            float digitX = startX + i * (digitWidth + style.digitSpacing);
            float digitY = bounds.y + (bounds.height - style.textSize) / 2;
            
            Rectangle digitRect = {digitX, digitY, digitWidth, (float)style.textSize};
            
            // Draw digit background (optional - can be removed for cleaner look)
            // DrawRectangleRec(digitRect, Color{30, 30, 30, 255});
            
            if (i < currentInput.length()) {
                // Draw actual digit
                const char* digitStr = std::string(1, currentInput[i]).c_str();
                int textWidth = MeasureText(digitStr, style.textSize);
                int textX = (int)(digitX + (digitWidth - textWidth) / 2);
                int textY = (int)(digitY + (style.textSize - style.textSize) / 2);
                
                DrawText(digitStr, textX, textY, style.textSize, style.textColor);
            } else if (style.showPlaceholder) {
                // Draw placeholder underscore
                const char* placeholder = "_";
                int textWidth = MeasureText(placeholder, style.textSize);
                int textX = (int)(digitX + (digitWidth - textWidth) / 2);
                int textY = (int)(digitY + (style.textSize - style.textSize) / 2);
                
                DrawText(placeholder, textX, textY, style.textSize, style.placeholderColor);
            }
        }
    }

    // Mouse interaction (optional - for future click-to-focus functionality)
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
        style = InputBoxStyle();
    }

    std::string currentInput;
    int maxDigits;
    InputBoxStyle style;
};
