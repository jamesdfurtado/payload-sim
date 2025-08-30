#pragma once

#include "../Widget.h"
#include "../widgets/PulsatingBorder.h"
#include <string>

class GuidanceView : public Widget {
public:
    GuidanceView() {
        // Initialize with empty guidance text for now
        guidanceText = "";
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
        
        // Only draw guidance text if it's not empty
        if (!guidanceText.empty()) {
            int fontSize = 18;
            Vector2 textSize = MeasureTextEx(GetFontDefault(), guidanceText.c_str(), fontSize, 1);
            
            float textX = bounds.x + (bounds.width - textSize.x) / 2;
            float textY = bounds.y + 25;  // Position below the label
            
            DrawText(guidanceText.c_str(), (int)textX, (int)textY, fontSize, WHITE);
        }
    }
    
    void update(float dt) override {
        pulsatingBorder.update(dt);
    }
    
    void setGuidanceText(const std::string& text) {
        guidanceText = text;
    }
    
    const std::string& getGuidanceText() const {
        return guidanceText;
    }

private:
    std::string guidanceText;
    PulsatingBorder pulsatingBorder;
};
