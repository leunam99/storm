#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"

TEST(BlackboxInterface, BlackboxWrapperOnWhiteboxCorrectModel) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/die_c1.nm");
    std::seed_seq seed({0});

    storm::modelchecker::blackbox::BlackboxWrapperOnWhitebox<uint32_t, double> wrapper(program);

    // two actions from init exist and have approximately correct distribution
    ASSERT_EQ(2, wrapper.getAvailActions(0));

    uint32_t totalSamples = 100000;
    std::vector<uint32_t> samplesA = {0, 0};
    std::vector<uint32_t> samplesB = {0, 0};
    for (int i = 0; i < totalSamples; i++) {
        uint32_t indexA = wrapper.sampleSucc(0, 0) - 1;
        samplesA[indexA]++;
        uint32_t indexB = wrapper.sampleSucc(0, 1) - 1;
        samplesB[indexB]++;
    }
    // assert we have not swapped the actions
    ASSERT_TRUE(wrapper.getActionLabels(0, 0).find("a") != wrapper.getActionLabels(0, 0).end());
    ASSERT_TRUE(wrapper.getActionLabels(0, 0).find("b") != wrapper.getActionLabels(0, 0).end());
    
    ASSERT_NEAR(samplesA[0] / (double) (samplesA[0] + samplesA[1]), 0.5, 0.1);
    double expectedProbB = 0.2;
    if (samplesB[0] > samplesB[1]) { expectedProbB = 0.8; }  // order of states is random, so we have to heck manually
    ASSERT_NEAR(samplesB[0] / (double) (samplesB[0] + samplesB[1]), expectedProbB, 0.1);

    // check reward model exists is correctly read 
    ASSERT_EQ(1, wrapper.getRewardModels().size());
    auto rewardModel = wrapper.getRewardModels()[0];
    ASSERT_EQ("coin_flips", rewardModel.getName());

    // init
    ASSERT_EQ(1, wrapper.getStateRewards(0).size());
    ASSERT_EQ(0, wrapper.getStateRewards(0)[0]);

    ASSERT_EQ(1, wrapper.getStateActionRewards(0, 0).size());
    ASSERT_EQ(0, wrapper.getStateActionRewards(0, 0)[0]);

    // first state
    ASSERT_EQ(1, wrapper.getStateRewards(1).size());
    ASSERT_EQ(0, wrapper.getStateRewards(1)[0]);

    ASSERT_EQ(1, wrapper.getStateActionRewards(1, 0).size());
    ASSERT_EQ(1, wrapper.getStateActionRewards(1, 0)[0]);
}