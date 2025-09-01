#pragma once

#include "../Widget.h"
#include "../../sim/SimulationEngine.h"
#include "../widgets/Indicator.h"
#include <memory>
#include <vector>

class StatusPanel : public Widget {
public:
    explicit StatusPanel(SimulationEngine& engine) : engine(engine) {
        // 3x3 grid of status lights
        const std::vector<std::string> labels = {
            "Authorization", "Target Validated", "Target Acquired",
            "Depth OK", "Tube Integrity", "Payload OK",
            "Power Stable", "No Friendlies", "Conditions OK"
        };
        
        for (const auto& label : labels) {
            indicators.push_back(std::make_unique<Indicator>(label, false));
        }
    }

    void draw() const override {
        const auto& s = engine.getState();
        
        DrawRectangleRec(bounds, Fade(DARKGRAY, 0.4f));
        
        //lights represent simulation state
        updateIndicatorStates(s);
        
        const int pad = 15;
        const int boxW = 180;
        const int boxH = 28;
        
        int x = (int)bounds.x + pad;
        int y = (int)bounds.y + pad;
        int index = 0;
        
        for (auto& indicator : indicators) {
            float adjustedY = (float)y;
            if (index >= 3) { 
                adjustedY -= 10;
            }
            if (index >= 6) {
                adjustedY -= 10;
            }
            
            indicator->setBounds({(float)x, adjustedY, (float)boxW, (float)boxH});
            indicator->draw();
            
            x += boxW + pad;
            if (x + boxW > bounds.x + bounds.width) {
                x = (int)bounds.x + pad;
                y += boxH + pad;
            }
            index++;
        }
    }

private:
    void updateIndicatorStates(const SimulationState& s) const {
        // wire sim values to lights
        indicators[0]->setState(s.canLaunchAuthorized);
        indicators[1]->setState(s.targetValidated);
        indicators[2]->setState(s.targetAcquired);
        indicators[3]->setState(s.depthClearanceMet);
        indicators[4]->setState(s.launchTubeIntegrity);
        indicators[5]->setState(s.payloadSystemOperational);
        indicators[6]->setState(s.powerSupplyStable);
        indicators[7]->setState(s.noFriendlyUnitsInBlastRadius);
        indicators[8]->setState(s.launchConditionsFavorable);
    }

    SimulationEngine& engine;
    mutable std::vector<std::unique_ptr<Indicator>> indicators;
};


