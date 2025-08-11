#pragma once

#include "../simulation/SimulationEngine.h"
#include <string>
#include <functional>

enum class LaunchPhase { Idle, Authorized, Arming, Armed, Launching, Launched, Resetting };

class SafetySystem : public ISimSystem {
public:
    const char* getName() const override { return "SafetySystem"; }
    void update(SimulationState& state, float dt) override;

    // Inputs coming from the UI / user
    void requestAuthorizationCode(const char* code);
    void arm();
    void launch();
    void reset(); 
    void forceIdle();

    LaunchPhase getPhase() const { return phase; }

    // Displays what triggered reset state
    const std::string& getResetReason() const { return resetReason; }

    // UI sets the current challenge code here
    void setChallengeCode(const std::string& code) { currentChallengeCode = code; }
    
    // Set callback for when reset completes (to turn off power)
    void setPowerOffCallback(std::function<void()> callback) { powerOffCallback = callback; }

private:
    // Current state
    LaunchPhase phase = LaunchPhase::Idle;
    float armingTimer = 0.0f;
    float launchingTimer = 0.0f;
    std::string resetReason;

    // Temporary event flags triggered by user actions
    // these are checked and cleared each update cycle
    bool eventAuthGranted = false;
    bool eventArm = false;
    bool eventLaunch = false;

    float resetTimer = 0.0f;

    std::string currentChallengeCode;
    
    // Callback to turn off power when reset completes
    std::function<void()> powerOffCallback;

    // Conditional checks to move to next state
    bool conditionsForAuthMet(const SimulationState& s) const;
    bool conditionsForArmMet(const SimulationState& s) const;
    bool conditionsForLaunchMet(const SimulationState& s) const;
    bool authorizedStateStillValid(const SimulationState& s) const;
    bool armedStateStillValid(const SimulationState& s) const;
};


