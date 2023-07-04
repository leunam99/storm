#include "storm/modelchecker/blackbox/EMdp2BMdp.h"

#include "storm/storage/SparseMatrix.h"


typedef storm::storage::SparseMatrixIndexType index_type;

using storm::models::sparse::BMdp;
using storm::modelchecker::blackbox::EMdp;

template <typename ValueType>
storm::storage::SparseMatrixBuilder<ValueType> initialiseMatrix(EMdp<int> &emdp){

    //calculate sizes of matrix
    //TODO keep track of (some of) this information in emdp?
    //colum count: number of states + 1 dummy state
    index_type states = 1 + emdp.getTotalStateCount();
    //row count: sum of number of actions for each state + 1 for dummy state
    index_type rows = 1 + emdp.gettotalStateActionPairCount();
    //entries: total number of nonzero entries + 1 for dummy state
    index_type entries = 1 + emdp.getTotalTransitionCount();

    return storm::storage::SparseMatrixBuilder<ValueType>(rows,states,entries,true,true,states);

}


//TODO make emdp (and therefore all called functions) const??
template <typename IndexType, typename ValueType>
BMdp<ValueType> infer(EMdp<int> &emdp, BoundFunc<ValueType> boundFunc, DeltaDistribution<IndexType> valueFunc, double pmin, double delta){

    using Bounds = storm::models::sparse::ValueTypePair<ValueType>;
    using new_index_t = typename storm::storage::SparseMatrixBuilder<ValueType>::index_type;

    //TODO state labeling, action labeling, rewards!
    storm::models::sparse::StateLabeling stateLabeling;
    //calculate size of BMdp and reserve enough space
    storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder = initialiseMatrix<Bounds>(emdp);

    // create a map that gives the states an order/index
    std::unordered_map<IndexType,new_index_t> index_mapping;

    new_index_t i = 0;
    for( auto states = emdp.getStateItr(); states.hasNext(); i++){
        index_mapping[states.next()] = i;
    }
    new_index_t dummy_state = i; //put dummy state last

    //initialise value function with eMDP
    valueFunc.initialiseFor(emdp, delta);


    int currentRow = 0;
    //for every state in the eMDP
    for(auto statesItr = emdp.getStateItr(); statesItr.hasNext(); ){
        auto state = statesItr.next();
        matrixBuilder.newRowGroup(currentRow);
        //for every action of this state
        for(auto actItr = emdp.getStateActionsItr(state); actItr.hasNext();) {
            auto action = actItr.next();
            int actionSamples = emdp.getSampleCount(state,action);

            if (actionSamples == 0) {
                // Account for unsampled actions: one transition to dummy state (interval [1,1])
                matrixBuilder.addNextValue(currentRow,dummy_state, storm::models::sparse::ValueTypePair<ValueType>::ValueTypePair(std::make_pair(1,1)));
            } else {
                // for every sampled target state
                for(auto targetItr = emdp.getStateActionsSuccItr(state,action); targetItr.hasNext();){
                    auto target_state = targetItr.next();
                    // call value function to get delta assigned to transition
                    double delta_transition = valueFunc.getDeltaT(state, action,target_state);

                    // get samples from eMDP
                    int samples = emdp.getSampleCount(state,action,target_state);

                    auto interval = boundFunc.INTERVAL(actionSamples, samples, delta_transition);
                    // insert into BMdp
                    matrixBuilder.addNextValue(currentRow,index_mapping[target_state],ValueTypePair(std::move(boundFunc)));
                }
            }

            //TODO in the greybox case, I think we need to consider the unsampled transition (that we know exist) here as well!
            // If blackbox: account for unsampled successors: transition to dummy state with the interval [0, 1]
            // skip if this state action pair has been sampled enough times
            bool isBlackbox = false;
            if(isBlackbox){
                //TODO is it correct to use the global delta? -> only calculate once
                //Otherwise: how can we make sure that the delta function takes these transitions into account?
                int requiredSamples = log(delta) / log(1 - pmin);
                if(actionSamples <= requiredSamples){
                    matrixBuilder.addNextValue(currentRow,dummy_state,ValueTypePair(std::make_pair(ValueType{0},ValueType{1})));
                }

            }

            currentRow++;
        }

    }

    //insert dummy state at the end of the matrix
    //TODO unknown reward: how??
    matrixBuilder.newRowGroup(currentRow);
    //one action with one transition to itself (interval [1,1])
    matrixBuilder.addNextValue(currentRow,dummy_state,ValueTypePair(std::make_pair(ValueType{1},ValueType{1})));

    //create BMdp
    return BMdp<ValueType>(matrixBuilder.build(), std::move(stateLabeling));

}