#include "LoggingSystem.h"
#include <algorithm>

LoggingSystem::LoggingSystem() {
}

void LoggingSystem::addInfo(const std::string& message, bool persistent) {
    addMessage(message, SKYBLUE, persistent);
}

void LoggingSystem::addWarning(const std::string& message, bool persistent) {
    addMessage(message, ORANGE, persistent);
}

void LoggingSystem::addError(const std::string& message, bool persistent) {
    addMessage(message, RED, persistent);
}

void LoggingSystem::addSuccess(const std::string& message, bool persistent) {
    addMessage(message, GREEN, persistent);
}

void LoggingSystem::addPhaseMessage(const std::string& phase, const std::string& reason) {
    std::string message = phase;
    if (!reason.empty()) {
        message += ": " + reason;
    }
    addMessage(message, YELLOW, false);
}

void LoggingSystem::addAuthSuccess() {
    addMessage("Authorization granted, please ARM the payload.", GREEN, true);
}

void LoggingSystem::addPayloadArmed() {
    addMessage("Payload armed.", GREEN, false);
}

void LoggingSystem::addSystemReset() {
    addMessage("System reset.", ORANGE, false);
}

void LoggingSystem::setPayloadState(const std::string& state) {
    currentPayloadState = state;
}

void LoggingSystem::addMessage(const std::string& message, Color color, bool persistent) {
    // Remove old non-persistent messages if we're at capacity
    if (messages.size() >= MAX_MESSAGES) {
        auto it = std::find_if(messages.begin(), messages.end(), 
            [](const LogMessage& msg) { return !msg.isPersistent; });
        if (it != messages.end()) {
            messages.erase(it);
        }
    }
    
    LogMessage logMsg;
    logMsg.message = message;
    logMsg.timestamp = GetTime();
    logMsg.color = color;
    logMsg.isPersistent = persistent;
    
    messages.push_back(logMsg);
}

void LoggingSystem::clearMessages() {
    messages.clear();
}

void LoggingSystem::clearNonPersistentMessages() {
    messages.erase(
        std::remove_if(messages.begin(), messages.end(),
            [](const LogMessage& msg) { return !msg.isPersistent; }),
        messages.end()
    );
}

void LoggingSystem::update(float dt) {
    float currentTime = GetTime();
    
    // Remove expired non-persistent messages
    messages.erase(
        std::remove_if(messages.begin(), messages.end(),
            [currentTime](const LogMessage& msg) {
                return !msg.isPersistent && (currentTime - msg.timestamp) > MESSAGE_LIFETIME;
            }),
        messages.end()
    );
}

void LoggingSystem::drawMessages(Rectangle area) {
    // Draw payload state at the top right of the brown box (moved 80 pixels left total)
    if (!currentPayloadState.empty()) {
        DrawText(TextFormat("Payload State: %s", currentPayloadState.c_str()), 
                (int)(area.x + area.width - 280), (int)area.y + 15, 20, YELLOW);
    }
    
    if (messages.empty()) return;
    
    // Only show the most recent message to prevent multiple messages appearing
    const LogMessage& msg = messages.back();
    int y = (int)area.y + 110; // Moved down 20 pixels from 90 to 110
    
    // Truncate long messages to fit in the area
    std::string displayMsg = msg.message;
    if (displayMsg.length() > 80) { // Limit to 80 characters
        displayMsg = displayMsg.substr(0, 77) + "...";
    }
    
    DrawText(displayMsg.c_str(), (int)area.x + 20, y, 14, LIGHTGRAY);
}
