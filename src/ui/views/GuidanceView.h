#pragma once

#include "../Widget.h"
#include "../widgets/PulsatingBorder.h"
#include "../../sim/world/MissionInstructionManager.h"
#include <string>

class GuidanceView : public Widget {
public:
    GuidanceView(const MissionInstructionManager& manager) 
        : missionManager(manager) {
        // Create pulsating border with stronger effect
        pulsatingBorder = PulsatingBorder(ORANGE, 3.0f, 0.3f, 1.0f, 2);
    }

    void draw() const override {
        // Use the pulsating border widget for stronger effect
        pulsatingBorder.drawFilledBorder(bounds, 0.2f);
        
        // Add inner glow effect
        Rectangle innerBounds = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
        DrawRectangleLinesEx(innerBounds, 1, Fade(YELLOW, 0.8f));
        
        // Draw "MISSION OBJECTIVES" label in top-left
        const char* headerText = "MISSION OBJECTIVES";
        int headerFontSize = 16;
        float headerX = bounds.x + 10;  // 10px from left edge
        float headerY = bounds.y + 8;   // 8px from top edge
        
        DrawText(headerText, (int)headerX, (int)headerY, headerFontSize, YELLOW);
        
        // Get current mission instruction and display it
        MissionInstruction instruction = missionManager.getCurrentInstruction();
        if (!instruction.instructionText.empty()) {
            int fontSize = 16;
            Vector2 textSize = MeasureTextEx(GetFontDefault(), instruction.instructionText.c_str(), fontSize, 1);
            
            float textX = bounds.x + 10;  // Left-aligned with header
            float textY = bounds.y + 30;  // Position below the header
            
            // Use different colors based on completion status
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
