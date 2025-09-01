#pragma once

#include "../../Widget.h"
#include "../../widgets/Button.h"
#include "../../../sim/systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include <memory>
#include <raylib.h>

class LaunchSequencePanel : public Widget {
public:
    explicit LaunchSequencePanel(LaunchSequenceHandler* handler);

    // Widget interface
    void draw() const override;
    bool onMouseDown(Vector2 mousePos) override;
    bool onMouseUp(Vector2 mousePos) override;
    bool onMouseMove(Vector2 mousePos) override;

    // State management
    void update(float dt);
    void setBounds(Rectangle newBounds);

private:
    LaunchSequenceHandler* sequenceHandler;
    
    std::unique_ptr<Button> authorizeButton;
    std::unique_ptr<Button> armButton;
    std::unique_ptr<Button> launchButton;
    std::unique_ptr<Button> resetButton;
    
    Rectangle buttonArea;
    
    void setupLayout();
    
    void onAuthorize();
    void onArm();
    void onLaunch();
    void onReset();
};
