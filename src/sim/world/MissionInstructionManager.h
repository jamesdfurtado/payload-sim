#pragma once

#include "../SimulationEngine.h"
#include "../systems/LaunchSequenceHandler/LaunchSequenceHandler.h"
#include "../systems/LaunchSequenceHandler/CurrentLaunchPhase.h"
#include <string>

enum class MissionStep {
    ADJUST_DEPTH,
    ACQUIRE_TARGET,
    TURN_ON_POWER,
    CLICK_AUTHORIZE,
    INPUT_AUTH_CODE,
    CLICK_ARM,
    CLICK_LAUNCH,
    MISSION_COMPLETE
};

enum class PulsateTarget {
    NONE,
    DEPTH_THROTTLE,
    SONAR_BOX,
    POWER_SWITCH,
    AUTHORIZE_BUTTON,
    KEYPAD_AREA,
    ARM_BUTTON,
    LAUNCH_BUTTON
};

struct MissionInstruction {
    MissionStep step;
    std::string instructionText;
    PulsateTarget pulsateTarget;
    bool isComplete;
};

class MissionInstructionManager {
public:
    MissionInstructionManager(SimulationEngine& engine, LaunchSequenceHandler* launchHandler)
        : engine(engine), launchSequenceHandler(launchHandler) {}

    MissionInstruction getCurrentInstruction() const {
        const auto& state = engine.getState();
        
        // Priority order checking:
        
        // 1. Check depth (highest priority)
        if (!state.depthClearanceMet) {
            return {
                MissionStep::ADJUST_DEPTH,
                "Adjust depth to operational level (105m)",
                PulsateTarget::DEPTH_THROTTLE,
                false
            };
        }
        
        // 2. Check target acquisition
        if (!state.targetAcquired) {
            return {
                MissionStep::ACQUIRE_TARGET,
                "Click on enemy target (red) to acquire",
                PulsateTarget::SONAR_BOX,
                false
            };
        }
        
        // 3. Check power
        if (!state.powerSupplyStable) {
            return {
                MissionStep::TURN_ON_POWER,
                "Turn on weapons power switch",
                PulsateTarget::POWER_SWITCH,
                false
            };
        }
        
        // 4. Check launch sequence phases
        if (launchSequenceHandler) {
            CurrentLaunchPhase currentPhase = launchSequenceHandler->getCurrentPhase();
            
            switch (currentPhase) {
                case CurrentLaunchPhase::Idle:
                    // Check if authorization is pending (code generated but not entered)
                    if (launchSequenceHandler->isAuthorizationPending()) {
                        return {
                            MissionStep::INPUT_AUTH_CODE,
                            "Enter authorization code: " + launchSequenceHandler->getAuthCode(),
                            PulsateTarget::KEYPAD_AREA,
                            false
                        };
                    } else {
                        // Need to click authorize to generate code
                        return {
                            MissionStep::CLICK_AUTHORIZE,
                            "Click AUTHORIZE LAUNCH button",
                            PulsateTarget::AUTHORIZE_BUTTON,
                            false
                        };
                    }
                    
                case CurrentLaunchPhase::Authorized:
                    return {
                        MissionStep::CLICK_ARM,
                        "Click ARM WEAPON button",
                        PulsateTarget::ARM_BUTTON,
                        false
                    };
                    
                case CurrentLaunchPhase::Arming:
                    return {
                        MissionStep::CLICK_ARM,
                        "Arming weapon... please wait",
                        PulsateTarget::ARM_BUTTON,
                        false
                    };
                    
                case CurrentLaunchPhase::Armed:
                    return {
                        MissionStep::CLICK_LAUNCH,
                        "Click LAUNCH PAYLOAD button",
                        PulsateTarget::LAUNCH_BUTTON,
                        false
                    };
                    
                case CurrentLaunchPhase::Launching:
                    return {
                        MissionStep::CLICK_LAUNCH,
                        "Launching... standby",
                        PulsateTarget::LAUNCH_BUTTON,
                        false
                    };
                    
                case CurrentLaunchPhase::Launched:
                    return {
                        MissionStep::MISSION_COMPLETE,
                        "Mission successful! Payload launched",
                        PulsateTarget::NONE,
                        true
                    };
                    
                case CurrentLaunchPhase::Resetting:
                    return {
                        MissionStep::ADJUST_DEPTH,
                        "System resetting... prepare for next mission",
                        PulsateTarget::NONE,
                        false
                    };
            }
        }
        
        // Fallback
        return {
            MissionStep::ADJUST_DEPTH,
            "Initialize systems",
            PulsateTarget::NONE,
            false
        };
    }
    
    bool shouldPulsate(PulsateTarget target) const {
        MissionInstruction instruction = getCurrentInstruction();
        return instruction.pulsateTarget == target && !instruction.isComplete;
    }

private:
    SimulationEngine& engine;
    LaunchSequenceHandler* launchSequenceHandler;
};
