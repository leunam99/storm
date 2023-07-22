#include "storm/modelchecker/blackbox/EMdp2BMdp.h"

#include "storm/storage/SparseMatrix.h"


typedef storm::storage::SparseMatrixIndexType index_type;



template <typename ValueType, typename IndexType>
storm::storage::SparseMatrixBuilder<ValueType> initialiseMatrix(EMdp<IndexType> &emdp){

    //colum count: number of states + 1 dummy state
    IndexType states = 1 + emdp.getTotalStateCount();
    //row count: sum of number of actions for each state + 1 for dummy state
    IndexType rows = 1 + emdp.gettotalStateActionPairCount();
    //entries: total number of nonzero entries + 1 for dummy state + 1 for each action to account for unsampled transitions
    //this overestimates the needed entries because some of them do not need the unsampled ones
    IndexType entries = 1 + emdp.getTotalTransitionCount() + emdp.gettotalStateActionPairCount();

    return storm::storage::SparseMatrixBuilder<ValueType>(rows,states,entries,false,true,states);

}
template <typename IndexType>
void transferStateInformation(std::vector<std::string>&& labels, storm::models::sparse::StateLabeling& stateLabeling, IndexType newState){
    for(const auto& label: labels ){
        if(!stateLabeling.containsLabel(label)){
            stateLabeling.addLabel(label);
        }
        stateLabeling.addLabelToState(label,newState);
    }
    //TODO rewards!
}

void transferActionInformation(storm::models::sparse::ChoiceLabeling& choiceLabeling, int currentRow, std::vector<std::string>&& labels) {
    for(const auto& label: labels){
        if(!choiceLabeling.containsLabel(label)){
            choiceLabeling.addLabel(label);
        }
        choiceLabeling.addLabelToChoice(label,currentRow);
    }
}


// TODO make emdp (and therefore all called functions) const??
template <typename IndexType, typename ValueType>
BMdp<ValueType> infer(EMdp<IndexType> &emdp, BoundFunc<ValueType> &boundFunc, DeltaDistribution<IndexType> &valueFunc, double pmin, double delta, bool isBlackbox){

    using Bounds = storm::utility::ValuePair<ValueType>;

    //TODO rewards!

    //Because of the dummy state both the state and action label need to be copied as it is not possible to change the item count of a labeling
    storm::models::sparse::StateLabeling stateLabeling(emdp.getTotalStateCount() + 1);
    storm::models::sparse::ChoiceLabeling choiceLabeling(emdp.gettotalStateActionPairCount() + 1); //TODO werden unsampled actions mitgezählt??
    //calculate size of BMdp and reserve enough space
    storm::storage::SparseMatrixBuilder<Bounds> matrixBuilder = initialiseMatrix<Bounds, IndexType>(emdp);

    IndexType dummy_state = emdp.getTotalStateCount(); //put dummy state at the last index

    //initialise value function with eMDP
    valueFunc.initialiseFor(emdp, delta);

    int currentRow = 0;

    for(IndexType state = 0; state < emdp.getTotalStateCount(); state++ ){
        matrixBuilder.newRowGroup(currentRow);

        std::cout << "state: " << state << "\n";

        transferStateInformation(emdp.getStateLabels(state), stateLabeling, state);

        for(auto actItr = emdp.getStateActionsItr(state); actItr.hasNext();) { //TODO because this is also just ascending, we could iterate differently instead!
            auto action = actItr.next();
            int actionSamples = emdp.getSampleCount(state,action);

            int sampledSuccessors = 0; //For determining if we sampled all successors of a greybox MDP

            transferActionInformation(choiceLabeling, currentRow, emdp.getActionLabels(state, action));

            std::cout << "\taction: " << action << "\n";

            if (actionSamples == 0) {
                // Account for unsampled actions: one transition to dummy state (interval [1,1])
                matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(1.0,1.0)));
                std::cout << "\t\tunsampled\n";
            } else {
                for(auto targetItr = emdp.getStateActionsSuccItr(state,action); targetItr.hasNext();){
                    auto target_state = targetItr.next();
                    sampledSuccessors++;

                    // delta assigned to this transition
                    double delta_transition = valueFunc.getDeltaT(state, action,target_state);
                    int samples = emdp.getSampleCount(state,action,target_state);
                    auto interval = boundFunc.INTERVAL(actionSamples, samples, delta_transition);

                    std::cout << "\t\ttransition: succ " << target_state << ", value" << storm::utility::ValuePair(interval) << "\n";

                    matrixBuilder.addNextValue(currentRow,target_state,storm::utility::ValuePair(std::move(interval)));
                }

                // account for unsampled successors: transition to dummy state with the interval [0, 1]
                if(isBlackbox){
                    //blackbox: skip if this state action pair has been sampled enough times
                    int requiredSamples =floor( log(delta) / log(1 - pmin));
                    if(actionSamples <= requiredSamples){
                        matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(ValueType{0},ValueType{1})));
                    }
                } else {
                    //greybox: skip if we got all successors
                    if(sampledSuccessors < emdp.getSuccCount(state,action)){
                        matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(ValueType{0},ValueType{1})));
                    }
                }
            }

            currentRow++;
        }
    }

    //insert dummy state at the end of the matrix
    //TODO unknown reward: how??
    matrixBuilder.newRowGroup(currentRow);
    storm::storage::BitVector v(emdp.getTotalStateCount() + 1);
    v.set(emdp.getTotalStateCount());
    stateLabeling.addUniqueLabel("dummy_state", v);
    //one action with one transition to itself (interval [1,1])
    matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(ValueType{1},ValueType{1})));

    return BMdp<ValueType>(matrixBuilder.build(),std::move(stateLabeling));

}


template BMdp<double> infer<uint32_t, double>(EMdp<uint32_t> &emdp, BoundFunc<double> &boundFunc, DeltaDistribution<uint32_t> &valueFunc, double pmin, double delta, bool isBlackbox);
template BMdp<double> infer<uint64_t, double>(EMdp<uint64_t> &emdp, BoundFunc<double> &boundFunc, DeltaDistribution<uint64_t> &valueFunc, double pmin, double delta, bool isBlackbox);
