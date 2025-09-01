#include <gtest/gtest.h>
#include "sim/systems/LaunchSequenceHandler/IdlePhase.h"
#include "sim/SimulationState.h"
#include <cctype>

class IdlePhaseTest : public ::testing::Test {
protected:
    SimulationState state;
    
    void SetUp() override {
        state.targetValidated = false;
        state.targetAcquired = false;
        state.depthClearanceMet = false;
        state.launchTubeIntegrity = false;
        state.powerSupplyStable = false;
        state.noFriendlyUnitsInBlastRadius = false;
        state.launchConditionsFavorable = false;
    }
};

TEST_F(IdlePhaseTest, CannotAuthorizeWithoutConditions) {
    auto result = IdlePhase::canAuthorize(state);
    EXPECT_FALSE(result.canAuthorize);
    EXPECT_NE(result.message.find("Target not validated"), std::string::npos);
    EXPECT_NE(result.message.find("Power supply unstable"), std::string::npos);
}

TEST_F(IdlePhaseTest, CanAuthorizeWhenAllConditionsMet) {
    state.targetValidated = true;
    state.targetAcquired = true;
    state.depthClearanceMet = true;
    state.launchTubeIntegrity = true;
    state.powerSupplyStable = true;
    state.noFriendlyUnitsInBlastRadius = true;
    state.launchConditionsFavorable = true;
    
    auto result = IdlePhase::canAuthorize(state);
    EXPECT_TRUE(result.canAuthorize);
    EXPECT_NE(result.message.find("All authorization conditions met"), std::string::npos);
}

TEST_F(IdlePhaseTest, IdentifiesSpecificFailureConditions) {
    state.targetValidated = true;
    state.targetAcquired = true;
    state.depthClearanceMet = true;
    state.launchTubeIntegrity = false;
    state.powerSupplyStable = true;
    state.noFriendlyUnitsInBlastRadius = true;
    state.launchConditionsFavorable = true;
    
    auto result = IdlePhase::canAuthorize(state);
    EXPECT_FALSE(result.canAuthorize);
    EXPECT_NE(result.message.find("Launch tube integrity compromised"), std::string::npos);
}

TEST_F(IdlePhaseTest, GeneratesValidAuthorizationCodes) {
    std::string code1 = IdlePhase::createCode();
    std::string code2 = IdlePhase::createCode();
    
    EXPECT_EQ(code1.length(), 4);
    EXPECT_EQ(code2.length(), 4);
    EXPECT_NE(code1, code2);
    
    for (char c : code1) {
        EXPECT_TRUE(std::isdigit(c));
    }
}
