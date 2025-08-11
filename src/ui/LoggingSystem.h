#pragma once

#include <raylib.h>
#include <string>
#include <vector>

class LoggingSystem {
public:
    struct LogMessage {
        std::string message;
        float timestamp;
        Color color;
        bool isPersistent;
    };

    LoggingSystem();
    
    // Add different types of messages
    void addInfo(const std::string& message, bool persistent = false);
    void addWarning(const std::string& message, bool persistent = false);
    void addError(const std::string& message, bool persistent = false);
    void addSuccess(const std::string& message, bool persistent = false);
    
    // Add phase-specific messages
    void addPhaseMessage(const std::string& phase, const std::string& reason = "");
    
    // Add authorization success message
    void addAuthSuccess();
    
    // Add payload armed message
    void addPayloadArmed();
    
    // Add system reset message
    void addSystemReset();
    
    // Set current payload state
    void setPayloadState(const std::string& state);
    
    // Clear messages
    void clearMessages();
    void clearNonPersistentMessages();
    
    // Draw messages in the specified area
    void drawMessages(Rectangle area);
    
    // Update (remove old messages)
    void update(float dt);

private:
    void addMessage(const std::string& message, Color color, bool persistent);
    std::vector<LogMessage> messages;
    std::string currentPayloadState;
    static constexpr float MESSAGE_LIFETIME = 5.0f; // 5 seconds for non-persistent messages
    static constexpr int MAX_MESSAGES = 3; // Maximum messages to display at once
};
