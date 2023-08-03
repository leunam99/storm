
#include "test/storm_gtest.h"
#include "storm/modelchecker/blackbox/boundFunctions/BoundFunc.h"

bool isInterval(std::pair<double, double> i){
    return i.first >= 0
        && i.second <= 1
        && i.first <= i.second;
}

double difference(std::pair<double, double> i){
    return i.second - i.first;
}

//TEST: Bound functions return sensible intervals
TEST(BoundFunctions, IsInterval) {

    HoeffDingBound<double> h;
    OneSidedHoeffDingBound<double> h2;


    ASSERT_TRUE(isInterval(h.INTERVAL(10,1,0.1)));
    ASSERT_TRUE(isInterval(h.INTERVAL(10,5,0.1)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,1,0.1)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,85,0.1)));

    ASSERT_TRUE(isInterval(h.INTERVAL(10,1,0.01)));
    ASSERT_TRUE(isInterval(h.INTERVAL(10,5,0.01)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,1,0.01)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,85,0.01)));

    ASSERT_TRUE(isInterval(h.INTERVAL(10,1,0.9)));
    ASSERT_TRUE(isInterval(h.INTERVAL(10,5,0.9)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,1,0.9)));
    ASSERT_TRUE(isInterval(h.INTERVAL(100,85,0.9)));

    ASSERT_TRUE(isInterval(h2.INTERVAL(10,1,0.1)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(10,5,0.1)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,1,0.1)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,85,0.1)));

    ASSERT_TRUE(isInterval(h2.INTERVAL(10,1,0.01)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(10,5,0.01)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,1,0.01)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,85,0.01)));

    ASSERT_TRUE(isInterval(h2.INTERVAL(10,1,0.9)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(10,5,0.9)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,1,0.9)));
    ASSERT_TRUE(isInterval(h2.INTERVAL(100,85,0.9)));

}

//TEST: more samples lead to smaller intervals
TEST(BoundFunctions, Converges){

    HoeffDingBound<double> h;
    OneSidedHoeffDingBound<double> h2;

    int samples = 10;
    double difference1, difference2;

    difference1 = difference(h.INTERVAL(samples, samples/2, 0.1));
    difference2 = difference(h2.INTERVAL(samples, samples/2, 0.1));

    for(int i = 0; i < 5; i++){
        samples *= 10;

        double newDifference1 = difference(h.INTERVAL(samples, samples/2, 0.1));
        double newDifference2 = difference(h2.INTERVAL(samples, samples/2, 0.1));

        ASSERT_GT(difference1, newDifference1);
        ASSERT_GT(difference2, newDifference2);

        difference1 = newDifference1;
        difference2 = newDifference2;

    }
}

//Test: smaller uncertainty leads to larger intervals
TEST(BoundFunctions, Uncertainty){

    HoeffDingBound<double> h;
    OneSidedHoeffDingBound<double> h2;

    int samples = 1000;
    double delta = 0.2;
    double difference1, difference2;

    difference1 = difference(h.INTERVAL(samples, samples/2, delta));
    difference2 = difference(h2.INTERVAL(samples, samples/2, delta));

    for(int i = 0; i < 5; i++){

        delta /= 10;

        double newDifference1 = difference(h.INTERVAL(samples, samples/2, delta));
        double newDifference2 = difference(h2.INTERVAL(samples, samples/2, delta));

        ASSERT_LE(difference1, newDifference1);
        ASSERT_LE(difference2, newDifference2);

        difference1 = newDifference1;
        difference2 = newDifference2;

    }
}