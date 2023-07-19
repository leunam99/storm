#include "storm/modelchecker/blackbox/EMdp2BMdp.h"

#include "storm/storage/SparseMatrix.h"


typedef storm::storage::SparseMatrixIndexType index_type;



template <typename ValueType>
storm::storage::SparseMatrixBuilder<ValueType> initialiseMatrix(EMdp<int> &emdp){

    //colum count: number of states + 1 dummy state
    index_type states = 1 + emdp.getTotalStateCount();
    //row count: sum of number of actions for each state + 1 for dummy state
    index_type rows = 1 + emdp.gettotalStateActionPairCount();
    //entries: total number of nonzero entries + 1 for dummy state
    index_type entries = 1 + emdp.getTotalTransitionCount();

    return storm::storage::SparseMatrixBuilder<ValueType>(rows,states,entries,true,true,states);

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
BMdp<ValueType> infer(EMdp<int> &emdp, BoundFunc<ValueType> &boundFunc, DeltaDistribution<IndexType> &valueFunc, double pmin, double delta, bool isBlackbox){

    using Bounds = storm::utility::ValuePair<ValueType>;
    using new_index_t = typename storm::storage::SparseMatrixBuilder<ValueType>::index_type;

    //TODO rewards!
    storm::models::sparse::StateLabeling stateLabeling(emdp.getTotalStateCount() + 1);
    storm::models::sparse::ChoiceLabeling choiceLabeling(emdp.gettotalStateActionPairCount() + 1); //TODO werden unsampled actions mitgezählt??
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

    for(auto statesItr = emdp.getStateItr(); statesItr.hasNext(); ){
        auto state = statesItr.next();
        matrixBuilder.newRowGroup(currentRow);

        transferStateInformation(emdp.getStateLabels(state), stateLabeling, index_mapping[state]);

        for(auto actItr = emdp.getStateActionsItr(state); actItr.hasNext();) {
            auto action = actItr.next();
            int actionSamples = emdp.getSampleCount(state,action);

            int sampledSuccessors = 0; //For determining if we sampled all successors of a greybox MDP

            transferActionInformation(choiceLabeling, currentRow, emdp.getActionLabels(state, action));

            if (actionSamples == 0) {
                // Account for unsampled actions: one transition to dummy state (interval [1,1])
                matrixBuilder.addNextValue(currentRow,dummy_state, storm::utility::ValuePair<ValueType>::ValuePair(std::make_pair(1,1)));
            } else {
                for(auto targetItr = emdp.getStateActionsSuccItr(state,action); targetItr.hasNext();){
                    auto target_state = targetItr.next();
                    sampledSuccessors++;

                    // delta assigned to this transition
                    double delta_transition = valueFunc.getDeltaT(state, action,target_state);
                    int samples = emdp.getSampleCount(state,action,target_state);
                    auto interval = boundFunc.INTERVAL(actionSamples, samples, delta_transition);

                    matrixBuilder.addNextValue(currentRow,index_mapping[target_state],ValueTypePair(std::move(interval)));
                }
            }

            // account for unsampled successors: transition to dummy state with the interval [0, 1]
            if(isBlackbox){
                //blackbox: skip if this state action pair has been sampled enough times
                int requiredSamples =floor( log(delta) / log(1 - pmin));
                if(actionSamples <= requiredSamples){
                    matrixBuilder.addNextValue(currentRow,dummy_state,ValueTypePair(std::make_pair(ValueType{0},ValueType{1})));
                }
            } else {
                //greybox: skip if we got all successors
                if(sampledSuccessors < emdp.getSuccCount(state,action)){
                    matrixBuilder.addNextValue(currentRow,dummy_state,ValueTypePair(std::make_pair(ValueType{0},ValueType{1})));
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
    matrixBuilder.addNextValue(currentRow,dummy_state,ValueTypePair(std::make_pair(ValueType{1},ValueType{1})));


    storm::storage::sparse::ModelComponents m(matrixBuilder.build(), std::move(stateLabeling));
    m.choiceLabeling = std::move(choiceLabeling);
    return BMdp<ValueType>(std::move(m));

}


template<> BMdp<double> infer<int_fast64_t,double>(EMdp<int> &emdp, BoundFunc<double> &boundFunc, DeltaDistribution<int_fast64_t> &valueFunc, double pmin, double delta, bool isBlackbox);