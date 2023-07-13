#include "modelchecker/blackbox/EMdp.h"
#include "modelchecker/blackbox/EMdp2BMdp.h"
#include "test/storm_gtest.h"

TEST(EMdp, CreationWithDimensions) {
    auto e = storm::modelchecker::blackbox::EMdp<int>();

    e.addInitialState(1);

    e.addState(1, {0,1,2});
    e.addState(2, {0});

    e.addVisits(1,0,1, 10);
    e.addVisits(1,0,2, 10);

    e.addVisits(1,1,1, 10);
    e.addVisits(1,1,2, 90);

    e.addVisits(1,2,1, 1000);
    e.addVisits(1,2,2, 9000);

    HoeffDingBound<double> bound;
    UniformDelta<int_fast32_t> delta;

    BMdp<double> bmdp = infer(e,bound,delta,0.01,0.1, true);
    bmdp.writeDotToStream(std::cout);

}
