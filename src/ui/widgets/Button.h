#pragma once

#include "../Widget.h"
#include <functional>
#include <string>
#include <raylib.h>

class Button : public Widget {
public:
    enum class State {
        Normal,     // Default state
        Hovered,    // Mouse hovering over button
        Pressed,    // Button being pressed
        Disabled,   // Button is disabled/greyed out
        Active,     // Button is active/highlighted
        Completed   // Button action completed
    };

    enum class Shape {
        Rectangle,  // Standard rectangular button
        Square      // Square button (for keypad)
    };

    struct ButtonStyle {
        Color backgroundColor = DARKGRAY;
        Color borderColor = RAYWHITE;
        Color textColor = RAYWHITE;
        Color disabledColor = GRAY;
        Color activeColor = GREEN;
        Color completedColor = BLUE;
        int borderThickness = 2;
        int textSize = 18;
        bool centerText = true;
    };

    // Constructor for general buttons
    explicit Button(std::string label, std::function<void()> onClick)
        : label(std::move(label)), onClick(std::move(onClick)), shape(Shape::Rectangle) {
        setupDefaultStyle();
    }

    // Constructor for keypad buttons
    Button(char key, std::function<void(char)> onKeyPress)
        : label(std::string(1, key)), onKeyPress(std::move(onKeyPress)), 
          shape(Shape::Square), isKeypadButton(true) {
        setupDefaultStyle();
    }

    // Constructor for sequence buttons
    Button(std::string label, std::function<void()> onClick, bool initiallyEnabled)
        : label(std::move(label)), onClick(std::move(onClick)), 
          shape(Shape::Rectangle), enabled(initiallyEnabled) {
        setupDefaultStyle();
    }

    // State management
    void setEnabled(bool enabled) { this->enabled = enabled; }
    void setState(State state) { this->state = state; }
    void setActive(bool active) { this->active = active; }
    void setCompleted(bool completed) { this->completed = completed; }

    // Style customization
    void setStyle(const ButtonStyle& style) { this->style = style; }
    void setColors(Color bg, Color border, Color text) {
        style.backgroundColor = bg;
        style.borderColor = border;
        style.textColor = text;
    }
    void setShape(Shape newShape) { shape = newShape; }
    void setTextSize(int size) { style.textSize = size; }
    void setBorderThickness(int thickness) { style.borderThickness = thickness; }

    // Getters
    bool isEnabled() const { return enabled; }
    State getState() const { return state; }
    bool isActive() const { return active; }
    bool isCompleted() const { return completed; }
    const std::string& getLabel() const { return label; }

    // Update method
    void update(float dt) {
        // Button state updates can go here if needed
        // For now, this is a placeholder for consistency with other widgets
    }

    void draw() const override {
        if (!enabled) {
            // Draw disabled state
            DrawRectangleRec(bounds, style.disabledColor);
            DrawRectangleLinesEx(bounds, style.borderThickness, GRAY);
            drawText(style.disabledColor);
            return;
        }

        // Determine colors based on state
        Color bgColor = style.backgroundColor;
        Color borderColor = style.borderColor;
        Color textColor = style.textColor;

        if (completed) {
            bgColor = style.completedColor;
            borderColor = style.completedColor;
        } else if (active) {
            bgColor = style.activeColor;
            borderColor = style.activeColor;
        } else if (state == State::Hovered) {
            bgColor = ColorBrightness(bgColor, 1.2f);
        } else if (state == State::Pressed) {
            bgColor = ColorBrightness(bgColor, 0.8f);
        }

        // Draw background and border
        DrawRectangleRec(bounds, bgColor);
        DrawRectangleLinesEx(bounds, style.borderThickness, borderColor);

        // Draw text
        drawText(textColor);
    }

    bool onMouseDown(Vector2 pos) override {
        if (!enabled || !contains(pos)) return false;
        
        state = State::Pressed;
        return true;
    }

    bool onMouseUp(Vector2 pos) override {
        if (!enabled) return false;
        
        if (contains(pos)) {
            if (state == State::Pressed) {
                // Handle different button types
                if (isKeypadButton && onKeyPress) {
                    onKeyPress(label[0]);
                } else if (onClick) {
                    onClick();
                }
                state = State::Normal;
                return true;
            }
        }
        
        state = State::Normal;
        return false;
    }

    bool onMouseMove(Vector2 pos) override {
        if (!enabled) return false;
        
        if (contains(pos)) {
            if (state != State::Pressed) {
                state = State::Hovered;
            }
        } else {
            state = State::Normal;
        }
        return false;
    }

private:
    void setupDefaultStyle() {
        style = ButtonStyle();
    }

    void drawText(Color textColor) const {
        if (label.empty()) return;

        int textWidth = MeasureText(label.c_str(), style.textSize);
        int textHeight = style.textSize;
        
        int textX, textY;
        
        if (style.centerText) {
            textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
            textY = (int)(bounds.y + (bounds.height - textHeight) / 2);
        } else {
            textX = (int)bounds.x + 5;
            textY = (int)(bounds.y + (bounds.height - textHeight) / 2);
        }

        DrawText(label.c_str(), textX, textY, style.textSize, textColor);
    }

    std::string label;
    std::function<void()> onClick;
    std::function<void(char)> onKeyPress;
    
    ButtonStyle style;
    Shape shape = Shape::Rectangle;
    State state = State::Normal;
    
    bool enabled = true;
    bool active = false;
    bool completed = false;
    bool isKeypadButton = false;
};


