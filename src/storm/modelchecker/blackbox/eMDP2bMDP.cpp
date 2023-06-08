#include "eMDP2bMDP.h"
#include "bMDP.h"
#include "eMDP.h"
#include "modelchecker/blackbox/bound-functions/boundFunc.h"
#include "modelchecker/blackbox/valueFunctions /valueFunc.h"
#include "storm/storage/SparseMatrix.h"

typedef storm::storage::SparseMatrixIndexType index_type;

template <typename ValueType>
storm::storage::SparseMatrixBuilder<ValueType> initialiseMatrix(const eMDP &emdp){

    //calculate sizes of matrix
    //colum count: number of states + 1 dummy state
    index_type states = 1;
    //row count: sum of number of actions for each state + 1 for dummy state
    index_type rows = 1;
    //entries: total number of nonzero entries + 1 for dummy state
    index_type entries = 1;

    return storm::storage::SparseMatrixBuilder<ValueType>(rows,states,entries,true,true,states);

}



template <typename IndexType, typename ValueType>
bMDP infer(const eMDP &emdp, BoundFunc<ValueType> boundFunc, ValueFunc<IndexType, ValueType> valueFunc, double pmin, double delta){

    using Bounds = ValueTypePair<ValueType,ValueType>;

    //TODO state labeling, action labeling, rewards!
    // also, we need to create a map that gives the states an order/index
    //calculate size of bMDP and reserve enough space
    storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder = initialiseMatrix<Bounds>(emdp);
    IndexType dummy_state; //get state index for this last state

    //initialise value function with eMDP
    //TODO like this, the valueFunc likely has to mirror the whole structure of the eMDP if it is more complicated than even
    // -> maybe save it in the eMDP and have a function to recalculate it (but this is waste for the simple case)
    valueFunc.initialiseFor(emdp, delta);

    int currentRow = 0;
    //for every state in the eMDP
    for(state){
        matrixBuilder.newRowGroup(currentRow);
        //for every action of this state
        for(action) {
            // sum up samples of this action
            int totalSamples;

            if (totalSamples == 0) {
                // Account for unsampled actions: one transition to dummy state (interval [1,1)
            } else {
                // for every sampled target state
                //TODO in the greybox case, I think we need to consider the unsampled transition (that we know exist) here as well!
                for(target_state){

                    // call value function to get delta assigned to transition
                    double delta_transition = valueFunc.getDeltaT(state, action,target_state);

                    // get samples from eMDP
                    int samples;

                    auto interval = boundFunc.INTERVAL(totalSamples, samples, delta);
                    // insert into bMDP
                    //TODO assumes target_state to be the index, probably is not like that!
                    matrixBuilder.addNextValue(currentRow,target_state,ValueTypePair(std::move(boundFunc)));

                }
            }
            // If blackbox: account for unsampled successors: transition to dummy state with the interval [0, 1]
            // TODO do we need to mark them for later figuring out the correct value?
            // skip if this state action pair has been sampled enough times
            if(blackbox){
                //TODO is it correct to use the global delta? -> only calculate once
                //Otherwise: how can we make sure that the delta function takes these transitions into account?
                int requiredSamples = log(delta) / log(1 - pmin);
                if(totalSamples <= requiredSamples){
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


    auto matrix = matrixBuilder.build();

    //create bMDP
    return bMDP();

}