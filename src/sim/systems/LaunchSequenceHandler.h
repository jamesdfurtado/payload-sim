#pragma once

class LaunchSequenceHandler {
public:
    LaunchSequenceHandler();
    ~LaunchSequenceHandler();

    // Request functions for UI button presses
    void requestAuthorization();
    void requestArm();
    void requestLaunch();
    void requestReset();

private:
    // TODO: Add state management and implementation details
};
