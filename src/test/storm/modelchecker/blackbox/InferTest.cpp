#include "modelchecker/blackbox/EMdp.h"
#include "modelchecker/blackbox/Infer.h"
#include "storm/modelchecker/blackbox/Simulator.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"
#include "test/storm_gtest.h"

using namespace storm::modelchecker::blackbox;

/*
 * Simulates knuth's die model until all states are visited,
 * then calls infer on it
 */
BMdp<double>  inferDie(){
    storm::prism::Program program;
    EXPECT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm"));
    auto ptr = std::make_shared<BlackboxWrapperOnWhitebox<uint32_t, double>>(program);
    auto blackboxMDP = std::static_pointer_cast<BlackboxMDP<uint32_t, double>>(ptr);
    Simulator<uint32_t, double> simulator(blackboxMDP, std::static_pointer_cast<heuristicSim::HeuristicSim<uint32_t, double>>(std::make_shared<heuristicSim::NaiveHeuristicSim<uint32_t, double>>(blackboxMDP)));

    EMdp<uint32_t> eMDP;

    //simulate until we got all states of the graph
    while(eMDP.getTotalStateCount() < 13)
        simulator.simulate(eMDP, 10);

    HoeffDingBound<double> boundfunc;
    UniformDelta<uint32_t> delta;

    return infer(eMDP,boundfunc, delta, 0.01, 0.01, false, blackboxMDP);

}




TEST(Infer, correctSize) {
    auto bMDP = inferDie();

    ASSERT_EQ(14ul, bMDP.getNumberOfStates()); //One extra dummy state

    //every state should have exactly one choice
    for(auto i = 0ul; i < 14ul; i++){
        ASSERT_EQ(bMDP.getNumberOfChoices(i), 1ul);
    }

    ASSERT_EQ(6ul, bMDP.getStates("done").getNumberOfSetBits());
    ASSERT_EQ(1ul, bMDP.getStates("one").getNumberOfSetBits());



}

TEST(Infer, rewards) {
    auto bMDP = inferDie();
    storm::models::sparse::StandardRewardModel<double> reward;
    ASSERT_NO_THROW(reward = bMDP.getRewardModel("coin_flips"));

    //Everything should be 0 except the dummy state
    bool nanFound = false;
    for(auto r : reward.getStateRewardVector()){
        if(std::isnan(r)){
            ASSERT_FALSE(nanFound);
            nanFound=true;
        } else {
            ASSERT_EQ(0, r);
        }
    }
    ASSERT_TRUE(nanFound);

    // There should be exactly 1 nan (dummy state)
    // 6 zeroes (final states) and the rest ones
    nanFound = false;
    int zeroes = 0;
    for(auto r : reward.getStateActionRewardVector()){
        if(r == 0){
            zeroes++;
        } else if(std::isnan(r)){
            ASSERT_FALSE(nanFound);
            nanFound=true;
        } else {
            ASSERT_EQ(1, r);
        }
    }
    ASSERT_TRUE(nanFound);
    ASSERT_EQ(zeroes, 6);

}

TEST(Infer, intervalsCorrect) {
    auto bMDP = inferDie();

    // Each transition should either lead to the dummy state or be a transition of the graph
    // These either have the actual probability 1 or 0.5 -> these should always be included

    for(auto entry : bMDP.getTransitionMatrix()){
        if(entry.getColumn() != bMDP.getNumberOfStates() -1){
            ASSERT_TRUE(0.5 >= entry.getValue().getLBound() || 1 <= entry.getValue().getUBound());
        }
    }

}


