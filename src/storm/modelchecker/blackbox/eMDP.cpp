#include <algorithm>

#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
EMdp<StateType>::EMdp() : explorationOrder(), hashStorage(), stateLabeling() {
}

template<typename StateType>
void EMdp<StateType>::addStateToExplorationOrder(StateType state) {
    if(explorationOrder.find(state) == explorationOrder.end())
        explorationOrder[state] = (explorationCount++);
}

template<typename StateType>
void EMdp<StateType>::print() {

    std::cout << "exploration order [state, explorationTime]:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";
    std::cout << "\nInitial State: " << initState << "\n";
    std::cout << "explored EMdp:\n";
    hashStorage.print();
}

//_______________________ Add states to EMdp ___________________________ //

template<typename StateType>
void EMdp<StateType>::addInitialState(StateType state) {
    if(initState == -1) 
        hashStorage.addState(state);
        initState = state;
}

template<typename StateType>
void EMdp<StateType>::addVisit(StateType state, StateType action, StateType succ) {
    addStateToExplorationOrder(succ);
    hashStorage.incTrans(state, action, succ, 1);
}

template<typename StateType>
void EMdp<StateType>::addVisits(StateType state, StateType action, StateType succ, StateType visits) {
    addStateToExplorationOrder(succ);
    hashStorage.incTrans(state, action, succ, visits);
}

template<typename StateType>
void EMdp<StateType>::addState(StateType state, std::vector<StateType> availActions) {
    addStateToExplorationOrder(state);
    hashStorage.addStateActions(state, availActions);
}

//_______________________ State/Trans Labeling Functions _______________________//

template<typename StateType>
void EMdp<StateType>::addStateLabel(std::string label, StateType state) {
    auto* labelVec = &stateLabeling[state];
    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it == labelVec->end())
        labelVec->push_back(label);
}

template<typename StateType>
void EMdp<StateType>::removeStateLabel(std::string label, StateType state) {
    auto* labelVec = &stateLabeling[state];

    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it != labelVec->end())
        labelVec->erase(it);
}

template<typename StateType>
std::vector<std::string> EMdp<StateType>::getStateLabels(StateType state) {
    if(stateLabeling.find(state) != stateLabeling.end())
        return stateLabeling[state];
    return std::vector<std::string>();
}

template<typename StateType>
void EMdp<StateType>::addActionLabel(std::string label, StateType state, StateType action) {
    auto* labelVec = &actionLabeling[std::make_pair(state, action)];
    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it == labelVec->end())
        labelVec->push_back(label);
}

template<typename StateType>   
void EMdp<StateType>::removeActionLabel(std::string label, StateType state, StateType action) {
    auto* labelVec = &actionLabeling[std::make_pair(state, action)];

    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it != labelVec->end())
        labelVec->erase(it);
}

template<typename StateType>
std::vector<std::string> EMdp<StateType>::getActionLabels(StateType state, StateType action) {
    if(actionLabeling.find(std::make_pair(state, action)) != actionLabeling.end())
        return actionLabeling[std::make_pair(state, action)];
    return std::vector<std::string>();
}

//______________________ Get/(Set) Count of Samples, Succ, Actions ____________________// 

template<typename StateType>
bool EMdp<StateType>::isStateKnown(StateType state) {
    return hashStorage.stateExists(state);
}

template<typename StateType>
StateType EMdp<StateType>::getTotalStateCount() {
    return hashStorage.getTotalStateCount();
}

template<typename StateType>
StateType EMdp<StateType>::gettotalStateActionPairCount() {
    return hashStorage.gettotalStateActionPairCount();
}

template<typename StateType>
StateType EMdp<StateType>::getTotalTransitionCount() {
    return hashStorage.getTotalTransitionCount();
}

template<typename StateType>
StateType EMdp<StateType>::getSampleCount(StateType state, StateType action) {
    return hashStorage.getTotalSamples(state, action);
}

template<typename StateType>
StateType EMdp<StateType>::getSampleCount(StateType state, StateType action, StateType succ) {
    return hashStorage.getSuccSamples(state, action, succ);
}

template<typename StateType>
void EMdp<StateType>::setSuccCount(StateType state, StateType action, StateType count) {
    hashStorage.setSuccCount(std::make_pair(state, action), count);
}

template<typename StateType>
StateType EMdp<StateType>::getSuccCount(StateType state, StateType action) {
    return hashStorage.getSuccCount(std::make_pair(state, action));
}

//___________________________ Get Iterators ____________________________//

template<typename StateType>
storage::KeyIterator<StateType> EMdp<StateType>::getStateItr() {
    return hashStorage.getStateItr();
}

template<typename StateType>
storage::KeyIterator<StateType> EMdp<StateType>::getStateActionsItr(StateType state) {
    return hashStorage.getStateActionsItr(state);
}

template<typename StateType>
storage::KeyIterator<StateType> EMdp<StateType>::getStateActionsSuccItr(StateType state, StateType action) {
    return hashStorage.getStateActionsSuccItr(state, action);
}

//__________________________ Create and acces reverse mapping ________________________________//

template<typename StateType>
void EMdp<StateType>::createReverseMapping() {
    hashStorage.createReverseMapping(); 
}

template<typename StateType>
std::vector<std::pair<StateType, StateType> > EMdp<StateType>::getPredecessors(StateType state) {  
    return hashStorage.getPredecessors(state);
}

template class EMdp<int_fast32_t>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

/*
int main(int argc, char const *argv[]) {
    auto EMdp = storm::modelchecker::blackbox::EMdp<int_fast32_t>();
    EMdp.addInitialState(1);
    
    EMdp.addStateLabel("label1", 1);
    EMdp.addStateLabel("label2", 1);
    EMdp.addStateLabel("label2", 1);
    EMdp.addStateLabel("label3", 1);
    
    for(auto x : EMdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }

    std::cout << "\n";
    EMdp.removeStateLabel("label1", 1);
    EMdp.removeStateLabel("label2", 1);

    for(auto x : EMdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }
    
    
    EMdp.writeDotFile("name.txt");
    return 0;
}
*/

