#pragma once

#include "../../Widget.h"
#include "../../widgets/Button.h"
#include <memory>
#include <vector>
#include <functional>
#include <raylib.h>

class KeypadPanel : public Widget {
public:
    using KeypadCallback = std::function<void(char)>;
    using BackspaceCallback = std::function<void()>;

    explicit KeypadPanel(KeypadCallback onKeyPress, BackspaceCallback onBackspace);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt) override;
    void setBounds(Rectangle newBounds);

private:
    // button handlers
    KeypadCallback keypadCallback;
    BackspaceCallback backspaceCallback;
    
    // keypad buttons
    std::vector<std::unique_ptr<Button>> keypadButtons;
    Rectangle keypadArea;

    void setupLayout();
    void createKeypad();
};
