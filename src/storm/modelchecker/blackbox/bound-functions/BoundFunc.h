#include <utility> 
#include <stdint.h>
#include <iostream>

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

enum BoundFuncType{HOEFFDING, ONESIDEDHOEFFDING};

/*!
 * Abstract Base Class for the function INTERVAL
 */
template <typename ValueType>
class BoundFunc {
   public:
    /*!
     * Calculates the lower and upper bound for a transition for the eMDP
     * @param totalSamples : total Samples for one Action generated in Simulate
     * @param partialSample : Samples for (state,transition,state) generated in Simulate
     * @param delta : Inconfidence value delta
     * @return : ValuePair with upper and lower bound
     */
    virtual std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) = 0; //abstract method
};

template <typename ValueType>
class HoeffDingBound : public BoundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) {
        ValueType bound_width = sqrt((log(delta / 2)) / (-2 * totalSamples));
        ValueType median = (ValueType)partialSample / (ValueType)totalSamples;
        return std::make_pair(std::clamp(median - bound_width, 0., 1.), std::clamp(median + bound_width, 0., 1.));
    }
};

template <typename ValueType>
class OneSidedHoeffDingBound : public BoundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) {
        ValueType bound_width = sqrt((log(delta / 2)) / (-2 * totalSamples));
        ValueType median = (ValueType)partialSample / (ValueType)totalSamples;
        return std::make_pair(std::clamp(median - bound_width, 0., 1.), 1.);
    } 
};

template <typename ValueType>
std::shared_ptr<BoundFunc<ValueType>> getBoundFunc(BoundFuncType type) {
    switch(type) {
        case BoundFuncType::HOEFFDING:
            return std::static_pointer_cast<BoundFunc<ValueType>>(std::make_shared<HoeffDingBound<ValueType>>());
        case BoundFuncType::ONESIDEDHOEFFDING:
            return std::static_pointer_cast<BoundFunc<ValueType>>(std::make_shared<OneSidedHoeffDingBound<ValueType>>());
    }
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "the selected boundary function " << type << "is not supported");
};
