#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/modelchecker/blackbox/BlackBoxExplorer.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"


TEST(BlackBoxExplorer, exploreSimpleMDP) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    std::seed_seq seed({0});
    auto blackboxMDP = std::static_pointer_cast<storm::modelchecker::blackbox::BlackboxMDP<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::BlackboxWrapperOnWhitebox<uint32_t, double>>(program));
    auto heuristic(std::static_pointer_cast<storm::modelchecker::blackbox::heuristicSim::HeuristicSim<uint32_t, double>>(std::make_shared<storm::modelchecker::blackbox::heuristicSim::NaiveHeuristicSim<uint32_t, double>>(blackboxMDP, seed)));

    storm::modelchecker::blackbox::BlackBoxExplorer<uint32_t, double> explorer(blackboxMDP, heuristic);
    storm::modelchecker::blackbox::EMdp<uint32_t> eMDP;
    explorer.performExploration(eMDP, 1);

    // expect a single was sampled of maximal 5 different states, more is not possible in two dice
    ASSERT_EQ(eMDP.getTotalStateCount(), 5);
    std::cout << "state COUNT" << eMDP.getTotalStateCount() << "\n";
    std::vector<uint32_t> states = {0, 1, 2, 3, 4, 5};

    ASSERT_EQ(eMDP.getSampleCount(0, 0, 1), 1);
    std::cout << "state COUNT" << eMDP.getTotalStateCount() << "\n";

    ASSERT_EQ(eMDP.getSampleCount(1, 0, 2), 1);
    if (eMDP.getSuccCount(2, 0) > 1) {
        // exploration looped in states 
    }
}

