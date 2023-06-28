#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
eMDP<StateType>::eMDP() : explorationOrder(), hashStorage(), stateLabeling() {
}

template<typename StateType>
void eMDP<StateType>::addStateToExplorationOrder(StateType state) {
    if(explorationOrder.find(state) == explorationOrder.end())
        explorationOrder[state] = (explorationCount++);
}

template<typename StateType>
void eMDP<StateType>::print() {

    std::cout << "exploration order [state, explorationTime]:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";
    std::cout << "\nInitial State: " << initState << "\n";
    std::cout << "explored eMDP:\n";
    hashStorage.print();
}

//_______________________ Add states to eMDP ___________________________ //

template<typename StateType>
void eMDP<StateType>::addInitialState(StateType state) {
    if(initState == -1) 
        hashStorage.addState(state);
        initState = state;
}

template<typename StateType>
void eMDP<StateType>::addVisit(StateType state, StateType action, StateType succ) {
    addStateToExplorationOrder(succ);
    hashStorage.incTrans(state, action, succ, 1);
}

template<typename StateType>
void eMDP<StateType>::addVisits(StateType state, StateType action, StateType succ, StateType visits) {
    addStateToExplorationOrder(succ);
    hashStorage.incTrans(state, action, succ, visits);
}

template<typename StateType>
void eMDP<StateType>::addState(StateType state, std::vector<StateType> availActions) {
    addStateToExplorationOrder(state);
    hashStorage.addStateActions(state, availActions);
}

//_______________________ State/Trans Labeling Functions _______________________//

template<typename StateType>
void eMDP<StateType>::addStateLabel(std::string label, StateType state) {
    auto* labelVec = &stateLabeling[state];
    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it == labelVec->end())
        labelVec->push_back(label);
}

template<typename StateType>
void eMDP<StateType>::removeStateLabel(std::string label, StateType state) {
    auto* labelVec = &stateLabeling[state];

    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it != labelVec->end())
        labelVec->erase(it);
}

template<typename StateType>
std::vector<std::string> eMDP<StateType>::getStateLabels(StateType state) {
    if(stateLabeling.find(state) != stateLabeling.end())
        return stateLabeling[state];
    return std::vector<std::string>();
}

template<typename StateType>
void eMDP<StateType>::addActionLabel(std::string label, StateType state, StateType action) {
    auto* labelVec = &actionLabeling[std::make_pair(state, action)];
    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it == labelVec->end())
        labelVec->push_back(label);
}

template<typename StateType>   
void eMDP<StateType>::removeActionLabel(std::string label, StateType state, StateType action) {
    auto* labelVec = &actionLabeling[std::make_pair(state, action)];

    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it != labelVec->end())
        labelVec->erase(it);
}

template<typename StateType>
std::vector<std::string> eMDP<StateType>::getActionLabels(StateType state, StateType action) {
    if(actionLabeling.find(std::make_pair(state, action)) != actionLabeling.end())
        return actionLabeling[std::make_pair(state, action)];
    return std::vector<std::string>();
}

//______________________ Get/(Set) Count of Samples, Succ, Actions ____________________// 

template<typename StateType>
bool eMDP<StateType>::isStateKnown(StateType state) {
    return hashStorage.stateExists(state);
}

template<typename StateType>
StateType eMDP<StateType>::getSampleCount(StateType state, StateType action) {
    return hashStorage.getTotalSamples(state, action);
}

template<typename StateType>
StateType eMDP<StateType>::getSampleCount(StateType state, StateType action, StateType succ) {
    return hashStorage.getSuccSamples(state, action, succ);
}

template<typename StateType>
void eMDP<StateType>::setSuccCount(StateType state, StateType action, StateType count) {
    hashStorage.setSuccCount(std::make_pair(state, action), count);
}

template<typename StateType>
StateType eMDP<StateType>::getSuccCount(StateType state, StateType action) {
    return hashStorage.getSuccCount(std::make_pair(state, action));
}

template<typename StateType> 
StateType eMDP<StateType>::getActionCount(StateType state) {
    return hashStorage.getActionCount(state);
}

//___________________________ Get Iterators ____________________________//

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::getStateItr() {
    return hashStorage.getStateItr();
}

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::getStateActionsItr(StateType state) {
    return hashStorage.getStateActionsItr(state);
}

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::getStateActionsSuccItr(StateType state, StateType action) {
    return hashStorage.getStateActionsSuccItr(state, action);
}

//__________________________ Create and acces reverse mapping ________________________________//

template<typename StateType>
void eMDP<StateType>::createReverseMapping() {
    hashStorage.createReverseMapping(); 
}

template<typename StateType>
std::vector<std::pair<StateType, StateType> > eMDP<StateType>::getPredecessors(StateType state) {  
    return hashStorage.getPredecessors(state);
}

template class eMDP<int_fast32_t>;
template class eMDP<int_fast64_t>;
} //namespace blackbox
} //namespace modelchecker
} //namespace storm

/*
int main(int argc, char const *argv[]) {
    auto emdp = storm::modelchecker::blackbox::eMDP<int_fast32_t>();
    emdp.addInitialState(1);
    
    emdp.addStateLabel("label1", 1);
    emdp.addStateLabel("label2", 1);
    emdp.addStateLabel("label2", 1);
    emdp.addStateLabel("label3", 1);
    
    for(auto x : emdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }

    std::cout << "\n";
    emdp.removeStateLabel("label1", 1);
    emdp.removeStateLabel("label2", 1);

    for(auto x : emdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }
    
    
    emdp.writeDotFile("name.txt");
    return 0;
}
*/

