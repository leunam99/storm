#ifndef STORM_VALUEFUNC_H
#define STORM_VALUEFUNC_H

template <typename IndexType, typename ValueType>
class ValueFunc { //TODO maybe DeltaDistribution is a better name for it

    /*!
     * lets this function initialise
     * @param emdp the eMDP for whose transitions the delta needs to be distributed
     * @param total_delta the wanted sum of all deltas
     */
    virtual void initialiseFor(const eMDP &emdp, double total_delta) = 0;

    /*!
     * returns the uncertainty for this specific transition
     * @param state
     * @param action
     * @param target_state
     * @return
     */
    virtual double getDeltaT(IndexType state, IndexType action, IndexType target_state) = 0;


};

#endif  // STORM_VALUEFUNC_H
