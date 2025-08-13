#pragma once

#include <string>
#include <vector>

enum class LaunchPhase {
    Idle,
    Authorization,
    Arming,
    Launch,
    Launched,
    Reset
};

struct AuthorizationResult {
    bool success;
    std::string message;
    std::vector<std::string> failedRequirements;
};
