#include "modelchecker/blackbox/EMdp.h"
#include "modelchecker/blackbox/infer.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "test/storm_gtest.h"

TEST(EMdp, CreationWithDimensions) {
    auto e = storm::modelchecker::blackbox::EMdp<uint32_t>();

    e.addInitialState(1);

    e.addState(0, {0,1,2});
    e.addState(1, {0});

    e.addVisits(0,0,0, 10);
    e.addVisits(0,0,1, 10);

    e.addVisits(0,1,0, 10);
    e.addVisits(0,1,1, 90);

    e.addVisits(0,2,0, 1000);
    e.addVisits(0,2,1, 9000);

    HoeffDingBound<double> bound;
    UniformDelta<uint32_t> delta;

    //storm::storage::sparse::ModelComponents<storm::utility::ValuePair<double>,storm::models::sparse::StandardRewardModel<double>> m{};

    //BMdp<double> bMdp = BMdp<double>(m);

    BMdp<double> bmdp = infer(e,bound,delta,0.01,0.1, true);
    bmdp.writeDotToStream(std::cout);
}



