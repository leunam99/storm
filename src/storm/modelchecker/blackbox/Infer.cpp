#include "storm/modelchecker/blackbox/Infer.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

typedef storm::storage::SparseMatrixIndexType index_type;

template <typename ValueType, typename IndexType>
storm::storage::SparseMatrixBuilder<ValueType> initialiseMatrix(EMdp<IndexType> &emdp){

    //colum count: number of states + 1 dummy state
    IndexType states = 1 + emdp.getTotalStateCount();
    //row count: sum of number of actions for each state + 1 for dummy state
    IndexType rows = 1 + emdp.gettotalStateActionPairCount();
    //entries: total number of nonzero entries + 1 for dummy state + 1 for each action to account for unsampled transitions
    //this overestimates the needed entries because for some of them we do not need the unsampled ones
    IndexType entries = 1 + emdp.getTotalTransitionCount() + emdp.gettotalStateActionPairCount();

    return storm::storage::SparseMatrixBuilder<ValueType>(rows,states,entries,false,true,states);

}
template <typename IndexType>
void transferStateInformation(std::vector<std::string> &&labels, storm::models::sparse::StateLabeling &stateLabeling, IndexType state,
                              const std::vector<double> &stateRewards, std::vector<storm::models::sparse::StandardRewardModel<double>> &newRewards) {
    //labels
    for(const auto& label: labels ){
        if(!stateLabeling.containsLabel(label)){
            stateLabeling.addLabel(label);
        }
        stateLabeling.addLabelToState(label, state);
    }

    //rewards
    for(size_t i = 0; i < stateRewards.size(); i++){
        newRewards[i].setStateReward(state, stateRewards[i]);
    }
}

void transferActionInformation(storm::models::sparse::ChoiceLabeling &choiceLabeling, int currentRow, std::vector<std::string> &&labels,
                               std::vector<storm::models::sparse::StandardRewardModel<double>> &newRewards, const std::vector<double> &actionRewards) {
    for(const auto& label: labels){
        if(!choiceLabeling.containsLabel(label)){
            choiceLabeling.addLabel(label);
        }
        choiceLabeling.addLabelToChoice(label,currentRow);
    }

    //rewards
    for(size_t i = 0; i < actionRewards.size(); i++){
        newRewards[i].setStateActionReward(currentRow, actionRewards[i]);
    }
}

/**
 * @param emdp the current simulation data
 * @param boundFunc function for calculating the intervals
 * @param valueFunc function for calculating deltas
 * @param pmin minimum probability of a transition
 * @param delta allowed uncertainty
 * @param isGreybox if this is a greyboy or blackbox automata
 * @param interface the oracle that returns the rewards of states and actions
 */
template <typename IndexType, typename ValueType, typename StateType>
BMdp<ValueType> infer(EMdp<IndexType> &emdp, BoundFunc<ValueType> &boundFunc, DeltaDistribution<IndexType> &valueFunc, double pmin, double delta, bool isGreybox, std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType, ValueType>> interface){

    using Bounds = storm::utility::ValuePair<ValueType>;

    //Because of the dummy state both the state and action label need to be copied as it is not possible to change the item count of a labeling
    storm::models::sparse::StateLabeling stateLabeling(emdp.getTotalStateCount() + 1);
    storm::models::sparse::ChoiceLabeling choiceLabeling(emdp.gettotalStateActionPairCount() + 1);

    //initialise vectors for rewards
    int amountOfRewardModels = interface->getRewardModels().size();
    std::vector<storm::models::sparse::StandardRewardModel<double>> rewards(amountOfRewardModels);
    for(int i = 0; i < amountOfRewardModels; i++){

        rewards[i] = storm::models::sparse::StandardRewardModel<double>(
            std::optional(std::vector<double>(1 + emdp.getTotalStateCount())),
            std::optional(std::vector<double>(1 + emdp.gettotalStateActionPairCount())));
    }

    //calculate size of BMdp and reserve enough space
    storm::storage::SparseMatrixBuilder<Bounds> matrixBuilder = initialiseMatrix<Bounds, IndexType>(emdp);

    IndexType dummy_state = emdp.getTotalStateCount(); //put dummy state at the last index

    //initialise value function with eMDP
    valueFunc.initialiseFor(emdp, delta);

    int currentRow = 0;

    for(IndexType state = 0; state < emdp.getTotalStateCount(); state++ ){
        matrixBuilder.newRowGroup(currentRow);

        transferStateInformation(emdp.getStateLabels(state), stateLabeling, state, interface->getStateRewards(state), rewards);

        for(auto actItr = emdp.getStateActionsItr(state); actItr.hasNext();) {
            auto action = actItr.next();
            int actionSamples = emdp.getSampleCount(state,action);

            int sampledSuccessors = 0; //For determining if we sampled all successors of a greybox MDP

            transferActionInformation(choiceLabeling, currentRow, emdp.getActionLabels(state, action), rewards,
                                      interface->getStateActionRewards(state, action));

            if (actionSamples == 0) {
                // Account for unsampled actions: one transition to dummy state (interval [1,1])
                matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(1.0,1.0)));
            } else {
                for(auto targetItr = emdp.getStateActionsSuccItr(state,action); targetItr.hasNext();){
                    auto target_state = targetItr.next();
                    sampledSuccessors++;

                    //Small optimisation: if we know that there is only one successor, we do not need to calculate anything and just set the interval to [1,1]
                    if(isGreybox && emdp.getSuccCount(state,action) == 1){
                        matrixBuilder.addNextValue(currentRow, target_state, storm::utility::ValuePair(std::make_pair(1.0,1.0)));
                    } else {
                        // delta assigned to this transition
                        double delta_transition = valueFunc.getDeltaT(state, action, target_state);
                        int samples = emdp.getSampleCount(state, action, target_state);
                        auto interval = boundFunc.INTERVAL(actionSamples, samples, delta_transition);

                        matrixBuilder.addNextValue(currentRow, target_state, storm::utility::ValuePair(std::move(interval)));
                    }
                }

                // account for unsampled successors: transition to dummy state with the interval [0, 1]
                if(!isGreybox){
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
    matrixBuilder.newRowGroup(currentRow);
    storm::storage::BitVector v(emdp.getTotalStateCount() + 1);
    v.set(emdp.getTotalStateCount());
    stateLabeling.addUniqueLabel("dummy_state", v);
    //one action with one transition to itself (interval [1,1])
    matrixBuilder.addNextValue(currentRow,dummy_state,storm::utility::ValuePair(std::make_pair(ValueType{1},ValueType{1})));

    for(auto & reward : rewards){
        reward.setStateReward(dummy_state, ValueType(NAN));
        reward.setStateActionReward(currentRow, ValueType(NAN));
    }

    //convert rewards to a map and create BMDP
    auto models = interface->getRewardModels();
    std::unordered_map<std::string,storm::models::sparse::StandardRewardModel<double>> rewardMap;
    for(int i = 0; i < amountOfRewardModels; i ++){
        rewardMap.insert(std::pair(models[i].getName(), std::move(rewards[i])));
    }

    storm::storage::sparse::ModelComponents<Bounds, storm::models::sparse::StandardRewardModel<ValueType> > components{matrixBuilder.build(),std::move(stateLabeling), std::move(rewardMap)};
    components.choiceLabeling = std::move(choiceLabeling);

    return BMdp<ValueType>(components);

}


template BMdp<double> infer<uint32_t, double, uint32_t>(EMdp<uint32_t> &emdp, BoundFunc<double> &boundFunc, DeltaDistribution<uint32_t> &valueFunc, double pmin, double delta, bool isBlackbox, std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<uint32_t, double>>);
template BMdp<double> infer<uint64_t, double, uint64_t>(EMdp<uint64_t> &emdp, BoundFunc<double> &boundFunc, DeltaDistribution<uint64_t> &valueFunc, double pmin, double delta, bool isBlackbox, std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<uint64_t, double>>);
