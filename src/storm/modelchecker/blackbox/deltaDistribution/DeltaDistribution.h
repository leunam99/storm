#ifndef STORM_DELTADISTRIBUTION_H
#define STORM_DELTADISTRIBUTION_H

#include "modelchecker/blackbox/EMdp.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

enum DeltaDistType{UNIFORM};

template <typename IndexType>
class DeltaDistribution {

    /*!
     * lets this function initialise
     * @param emdp the eMDP for whose transitions the delta needs to be distributed
     * @param total_delta the wanted sum of all deltas
     */
    virtual void initialiseFor(const storm::modelchecker::blackbox::EMdp<int> &emdp, double total_delta) = 0;

    /*!
     * returns the uncertainty for this specific transition
     * @param state
     * @param action
     * @param target_state
     * @return
     */
    virtual double getDeltaT(IndexType state, IndexType action, IndexType target_state) = 0;


};


template <typename IndexType>
class UniformDelta : public DeltaDistribution<IndexType> {

    /*!
     * lets this function initialise
     * @param emdp the eMDP for whose transitions the delta needs to be distributed
     * @param total_delta the wanted sum of all deltas
     */
     void initialiseFor(storm::modelchecker::blackbox::EMdp<int> &emdp, double total_delta) override {
         transitionDelta = total_delta /  emdp.getTotalTransitionCount();
     }

    /*!
     * returns the uncertainty for this specific transition
     * @param state
     * @param action
     * @param target_state
     * @return
     */
    double getDeltaT(IndexType state, IndexType action, IndexType target_state) override {
        return transitionDelta;
    }
   private:
    double transitionDelta;
};

template <typename IndexType>
std::shared_ptr<DeltaDistribution<IndexType>> getDeltaDistribution(DeltaDistType type) {
    switch(type) {
        case: DeltaDistType::UNIFORM:
            return static_pointer_cast<DeltaDistribution<IndexType>>(std::make_shared<UniformDelta<IndexType>>());
    }
    STORM_LOG_THROW(true, storm::exceptions::NotSupportedException, "the selected delta distribution " << type << "is not supported");
};




#endif  // STORM_DELTADISTRIBUTION_H
