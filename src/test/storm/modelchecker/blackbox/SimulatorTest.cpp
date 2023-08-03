#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/modelchecker/blackbox/Simulator.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/heuristicSimulate/HeuristicSim.h"


TEST(Simulator, simulateSimplePath) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin_flips.nm");
    std::seed_seq seed({0});
    auto blackboxMDP = std::static_pointer_cast<storm::modelchecker::blackbox::BlackboxMDP<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::BlackboxWrapperOnWhitebox<uint32_t, double>>(program));
    auto heuristic(std::static_pointer_cast<storm::modelchecker::blackbox::heuristicSim::HeuristicSim<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::heuristicSim::NaiveHeuristicSim<uint32_t, double>>(blackboxMDP, seed)));

    storm::modelchecker::blackbox::Simulator<uint32_t, double> simulator(blackboxMDP, heuristic);
    storm::modelchecker::blackbox::EMdp<uint32_t> eMDP;
    simulator.simulate(eMDP, 1);

    // simply expect a path i -> (i+1), each one sampled one time, last one is not checked as it can be a sink state with arbitrary self loops
    // if the state has a self loop we have to stop iterating, because from here on we don't know anymore how many samples it's supposed to have
    
    for (uint32_t i = 0; i < eMDP.getTotalStateCount() - 2; i++) {
        if (eMDP.getSampleCount(i, 0, i) >= 1) break;
        uint32_t flipAction = 0;
        if (eMDP.getActionLabels(i, 0).empty()) {  // always two actions possible, only one is labled
            flipAction = 1;
        }
        ASSERT_EQ(eMDP.getSampleCount(i, flipAction, i+1), 1);
        ASSERT_EQ(eMDP.getSampleCount(i, flipAction), 1);
    }
}
