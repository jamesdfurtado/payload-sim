#pragma once

#include "../../Widget.h"
#include "../../widgets/InputBox.h"
#include <memory>
#include <string>
#include <functional>
#include <raylib.h>

class AuthCodePanel : public Widget {
public:
    using AuthCodeCallback = std::function<void(const std::string&)>;

    explicit AuthCodePanel(AuthCodeCallback onAuthCodeSubmit);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt);
    void setBounds(Rectangle newBounds);
    
    // Public interface
    void setAuthCode(const std::string& code);
    void clearInput();
    void setInputBorderColor(Color color);
    bool isInputComplete() const;
    std::string getInputValue() const;
    
    // Keypad input handling
    void handleKeypadInput(char key);
    void handleBackspace();

private:
    // Callback
    AuthCodeCallback authCodeCallback;
    
    // Input boxes
    std::unique_ptr<InputBox> authCodeInput;
    std::unique_ptr<InputBox> authCodeDisplay;
    
    // Layout
    Rectangle authArea;
    
    // State
    std::string currentAuthCode;
    bool authCodeEntered;
    
    // Helper methods
    void setupLayout();
    void handleAuthCodeSubmit();
};
