#ifndef STORM_DELTADISTRIBUTION_H
#define STORM_DELTADISTRIBUTION_H

#include "modelchecker/blackbox/EMdp.h"
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






#endif  // STORM_DELTADISTRIBUTION_H
