#pragma once

#include "../Widget.h"
#include "../widgets/PulsatingBorder.h"
#include "../../sim/world/MissionInstructionManager.h"
#include <string>

class GuidanceView : public Widget {
public:
    GuidanceView(const MissionInstructionManager& manager) 
        : missionManager(manager) {
        // pulsate areas of interest for mission guidance
        pulsatingBorder = PulsatingBorder(ORANGE, 3.0f, 0.3f, 1.0f, 2);
    }

    void draw() const override {
        pulsatingBorder.drawFilledBorder(bounds, 0.2f);
        
        Rectangle innerBounds = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
        DrawRectangleLinesEx(innerBounds, 1, Fade(YELLOW, 0.8f));
        
        const char* headerText = "MISSION OBJECTIVES";
        int headerFontSize = 16;
        float headerX = bounds.x + 10;
        float headerY = bounds.y + 8;
        
        DrawText(headerText, (int)headerX, (int)headerY, headerFontSize, YELLOW);
        
        // show current mission step
        MissionInstruction instruction = missionManager.getCurrentInstruction();
        if (!instruction.instructionText.empty()) {
            int fontSize = 16;
            Vector2 textSize = MeasureTextEx(GetFontDefault(), instruction.instructionText.c_str(), fontSize, 1);
            
            float textX = bounds.x + 10;
            float textY = bounds.y + 30;
            
            // green when done, white when active
            Color textColor = instruction.isComplete ? GREEN : WHITE;
            DrawText(instruction.instructionText.c_str(), (int)textX, (int)textY, fontSize, textColor);
        }
    }
    
    void update(float dt) override {
        pulsatingBorder.update(dt);
    }
    
private:
    const MissionInstructionManager& missionManager;
    PulsatingBorder pulsatingBorder;
};
