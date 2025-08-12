#pragma once

#include "../Widget.h"
#include "../../sim/SimulationEngine.h"
#include "../widgets/Indicator.h"
#include <memory>
#include <vector>

class StatusPanel : public Widget {
public:
    explicit StatusPanel(SimulationEngine& engine) : engine(engine) {
        // Create 9 indicator widgets in a 3x3 grid
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
        
        // Draw light grey background panel
        DrawRectangleRec(bounds, Fade(DARKGRAY, 0.4f));
        
        // Update indicator states from simulation
        updateIndicatorStates(s);
        
        // Position and draw indicators in 3x3 grid
        const int pad = 10;
        const int boxW = 150;
        const int boxH = 28;
        
        int x = (int)bounds.x + pad;
        int y = (int)bounds.y + pad;
        int index = 0;
        
        for (auto& indicator : indicators) {
            indicator->setBounds({(float)x, (float)y, (float)boxW, (float)boxH});
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
        // Map simulation state to indicator states
        indicators[0]->setState(s.canLaunchAuthorized);        // Authorization
        indicators[1]->setState(s.targetValidated);            // Target Validated
        indicators[2]->setState(s.targetAcquired);             // Target Acquired
        indicators[3]->setState(s.depthClearanceMet);          // Depth OK
        indicators[4]->setState(s.launchTubeIntegrity);        // Tube Integrity
        indicators[5]->setState(s.payloadSystemOperational);   // Payload OK
        indicators[6]->setState(s.powerSupplyStable);          // Power Stable
        indicators[7]->setState(s.noFriendlyUnitsInBlastRadius); // No Friendlies
        indicators[8]->setState(s.launchConditionsFavorable);  // Conditions OK
    }

    SimulationEngine& engine;
    mutable std::vector<std::unique_ptr<Indicator>> indicators;
};


