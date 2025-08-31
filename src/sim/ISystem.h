#pragma once

#include <string>
#include "SimulationState.h"

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual const char* getName() const = 0;
    virtual void update(SimulationState& state, float dt) = 0;
};