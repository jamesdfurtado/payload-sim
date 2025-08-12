#pragma once

#include "../ISystem.h"
#include <string>
#include <functional>

enum class LaunchPhase { Idle, Authorized, Arming, Armed, Launching, Launched, Resetting };

class SafetySystem : public ISystem {
public:
    const char* getName() const override { return "SafetySystem"; }
    void update(SimulationState& state, float dt) override;

    void requestAuthorizationCode(const char* code);
    void arm();
    void launch();
    void reset();
    void forceIdle();

    LaunchPhase getPhase() const { return phase; }
    const std::string& getResetReason() const { return resetReason; }
    void setChallengeCode(const std::string& code) { currentChallengeCode = code; }
    void setPowerOffCallback(std::function<void()> callback) { powerOffCallback = callback; }

private:
    LaunchPhase phase = LaunchPhase::Idle;
    float armingTimer = 0.0f;
    float launchingTimer = 0.0f;
    float resetTimer = 0.0f;
    std::string resetReason;
    std::string currentChallengeCode;
    bool eventAuthGranted = false;
    bool eventArm = false;
    bool eventLaunch = false;
    std::function<void()> powerOffCallback;

    bool conditionsForAuthMet(const SimulationState& s) const;
    bool conditionsForArmMet(const SimulationState& s) const;
    bool conditionsForLaunchMet(const SimulationState& s) const;
    bool authorizedStateStillValid(const SimulationState& s) const;
    bool armedStateStillValid(const SimulationState& s) const;
};


