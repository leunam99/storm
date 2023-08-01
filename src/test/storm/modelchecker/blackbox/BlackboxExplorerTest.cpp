#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/modelchecker/blackbox/BlackBoxExplorer.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"


TEST(BlackBoxExplorer, exploreSimplePath) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    std::seed_seq seed({0});
    auto blackboxMDP = std::static_pointer_cast<storm::modelchecker::blackbox::BlackboxMDP<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::BlackboxWrapperOnWhitebox<uint32_t, double>>(program));
    auto heuristic(std::static_pointer_cast<storm::modelchecker::blackbox::heuristicSim::HeuristicSim<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::heuristicSim::NaiveHeuristicSim<uint32_t, double>>(blackboxMDP, seed)));

    storm::modelchecker::blackbox::BlackBoxExplorer<uint32_t, double> explorer(blackboxMDP, heuristic);
    storm::modelchecker::blackbox::EMdp<uint32_t> eMDP;
    explorer.performExploration(eMDP, 1);

    // simply expect a path i -> (i+1), each one sampled one time, last one is not checked as it can be a sink state with arbitrary self loops
    uint32_t total_states = eMDP.getTotalStateCount();
    for (uint32_t i = 0; i < eMDP.getTotalStateCount() - 2; i++) {
        ASSERT_EQ(eMDP.getSampleCount(i, 0, i+1), 1);
        ASSERT_EQ(eMDP.getSampleCount(i, 0), 1);
    }
}
