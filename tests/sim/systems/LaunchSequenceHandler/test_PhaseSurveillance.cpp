#include <gtest/gtest.h>
#include "sim/systems/LaunchSequenceHandler/PhaseSurveillance.h"
#include "sim/SimulationState.h"

class PhaseSurveillanceTest : public ::testing::Test {
protected:
    PhaseSurveillance surveillance;
    SimulationState state;
    
    void SetUp() override {
        state.targetValidated = true;
        state.targetAcquired = true;
        state.depthClearanceMet = true;
        state.launchTubeIntegrity = true;
        state.powerSupplyStable = true;
        state.noFriendlyUnitsInBlastRadius = true;
        state.launchConditionsFavorable = true;
    }
};

TEST_F(PhaseSurveillanceTest, EmptySurveillancePassesAllConditions) {
    auto result = surveillance.checkConditions(state);
    EXPECT_TRUE(result.isAuthorized);
    EXPECT_EQ(surveillance.getConditionCount(), 0);
}

TEST_F(PhaseSurveillanceTest, AddsSingleConditionAndEvaluates) {
    surveillance.addCondition([](const SimulationState& s) { return s.powerSupplyStable; }, 
                             "Power unstable");
    
    EXPECT_EQ(surveillance.getConditionCount(), 1);
    
    auto result = surveillance.checkConditions(state);
    EXPECT_TRUE(result.isAuthorized);
    EXPECT_NE(result.message.find("All authorization maintenance conditions met"), std::string::npos);
}

TEST_F(PhaseSurveillanceTest, DetectsSingleFailingCondition) {
    surveillance.addCondition([](const SimulationState& s) { return s.powerSupplyStable; }, 
                             "Power unstable");
    
    state.powerSupplyStable = false;
    auto result = surveillance.checkConditions(state);
    EXPECT_FALSE(result.isAuthorized);
    EXPECT_NE(result.message.find("Power unstable"), std::string::npos);
}

TEST_F(PhaseSurveillanceTest, EvaluatesMultipleConditions) {
    surveillance.addCondition([](const SimulationState& s) { return s.powerSupplyStable; }, 
                             "Power unstable. ");
    surveillance.addCondition([](const SimulationState& s) { return s.targetValidated; }, 
                             "Target invalid. ");
    surveillance.addCondition([](const SimulationState& s) { return s.launchTubeIntegrity; }, 
                             "Tube compromised. ");
    
    EXPECT_EQ(surveillance.getConditionCount(), 3);
    
    auto result = surveillance.checkConditions(state);
    EXPECT_TRUE(result.isAuthorized);
}

TEST_F(PhaseSurveillanceTest, AccumulatesMultipleFailureMessages) {
    surveillance.addCondition([](const SimulationState& s) { return s.powerSupplyStable; }, 
                             "Power unstable. ");
    surveillance.addCondition([](const SimulationState& s) { return s.targetValidated; }, 
                             "Target invalid. ");
    
    state.powerSupplyStable = false;
    state.targetValidated = false;
    
    auto result = surveillance.checkConditions(state);
    EXPECT_FALSE(result.isAuthorized);
    EXPECT_NE(result.message.find("Power unstable"), std::string::npos);
    EXPECT_NE(result.message.find("Target invalid"), std::string::npos);
}

TEST_F(PhaseSurveillanceTest, ClearsAllConditions) {
    surveillance.addCondition([](const SimulationState& s) { return s.powerSupplyStable; }, 
                             "Power unstable");
    surveillance.addCondition([](const SimulationState& s) { return s.targetValidated; }, 
                             "Target invalid");
    
    EXPECT_EQ(surveillance.getConditionCount(), 2);
    
    surveillance.clearConditions();
    EXPECT_EQ(surveillance.getConditionCount(), 0);
    
    state.powerSupplyStable = false;
    auto result = surveillance.checkConditions(state);
    EXPECT_TRUE(result.isAuthorized);
}

TEST_F(PhaseSurveillanceTest, HandlesDynamicConditionChanges) {
    surveillance.addCondition([](const SimulationState& s) { return s.noFriendlyUnitsInBlastRadius; }, 
                             "Friendly units in blast radius");
    
    auto result1 = surveillance.checkConditions(state);
    EXPECT_TRUE(result1.isAuthorized);
    
    state.noFriendlyUnitsInBlastRadius = false;
    auto result2 = surveillance.checkConditions(state);
    EXPECT_FALSE(result2.isAuthorized);
    
    state.noFriendlyUnitsInBlastRadius = true;
    auto result3 = surveillance.checkConditions(state);
    EXPECT_TRUE(result3.isAuthorized);
}
