#pragma once

#include "../../Widget.h"
#include "../../widgets/TextBox.h"
#include <memory>
#include <string>
#include <functional>
#include <raylib.h>

class AuthCodePanel : public Widget {
public:
    using AuthCodeCallback = std::function<void(const std::string&)>;

    explicit AuthCodePanel(AuthCodeCallback onAuthCodeSubmit);

    // widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // state management
    void update(float dt);
    void setBounds(Rectangle newBounds);
    
    // public interface
    void setAuthCode(const std::string& code);
    void clearInput();
    void clearAuthCodeDisplay();
    void setInputBorderColor(Color color);
    bool isInputComplete() const;
    std::string getInputValue() const;
    
    // keypad input handling
    void handleKeypadInput(char key);
    void handleBackspace();

private:
    // submit handler
    AuthCodeCallback authCodeCallback;
    
    // input and display boxes
    std::unique_ptr<TextBox> inputBox;
    std::unique_ptr<TextBox> displayBox;
    
    // positioning
    Rectangle authArea;
    
    // current values
    std::string currentAuthCode;
    bool authCodeEntered;
    
    // internal helpers
    void setupLayout();
    void handleAuthCodeSubmit();
};
