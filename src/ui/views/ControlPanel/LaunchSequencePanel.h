#pragma once

#include "../../Widget.h"
#include "../../widgets/Button.h"
#include <memory>
#include <functional>
#include <raylib.h>

class LaunchSequencePanel : public Widget {
public:
    using AuthorizeCallback = std::function<void()>;
    
    explicit LaunchSequencePanel(AuthorizeCallback onAuthorize = nullptr);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt);
    void setBounds(Rectangle newBounds);

private:
    AuthorizeCallback authorizeCallback;
    
    // State buttons
    std::unique_ptr<Button> authorizeButton;
    std::unique_ptr<Button> armButton;
    std::unique_ptr<Button> launchButton;
    std::unique_ptr<Button> resetButton;
    
    // Layout
    Rectangle buttonArea;
    
    // Helper methods
    void setupLayout();
    
    // Button callbacks
    void onAuthorize();
    void onArm();
    void onLaunch();
    void onReset();
};
