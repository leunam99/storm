#ifndef STORM_DELTADISTRIBUTION_H
#define STORM_DELTADISTRIBUTION_H

#include "modelchecker/blackbox/eMDP.h"
template <typename IndexType>
class DeltaDistribution {

    /*!
     * lets this function initialise
     * @param emdp the eMDP for whose transitions the delta needs to be distributed
     * @param total_delta the wanted sum of all deltas
     */
    virtual void initialiseFor(const storm::modelchecker::blackbox::eMDP<int> &emdp, double total_delta) = 0;

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
     void initialiseFor(storm::modelchecker::blackbox::eMDP<int> &emdp, double total_delta) override {
        int transitions = 0;

         for(auto state: emdp.get_state_vec()){
             for(auto action : emdp.get_state_actions_vec(state)){
                 transitions += emdp.get_state_action_succ_vec(state,action).size();
             }
         }

         transitionDelta = total_delta / transitions;
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
