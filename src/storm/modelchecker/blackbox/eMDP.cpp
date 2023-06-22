#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
eMDP<StateType>::eMDP() : explorationOrder(), hashStorage(), stateLabeling() {
}

template<typename StateType>
void eMDP<StateType>::addInitialState(StateType state) {
    if(init_state == -1) 
        hashStorage.add_state(state);
        init_state = state;
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
    std::cout << "\nInitial State: " << init_state << "\n";
    std::cout << "explored eMDP:\n";
    hashStorage.print();
}

template<typename StateType>
std::string eMDP<StateType>::toDotString() {
    std::unordered_map<int, std::string> color_map; //colors to distinguish actions 
    color_map[0] = "green";
    color_map[1] = "red";
    color_map[2] = "blue";
    color_map[3] = "yellow";
    color_map[4] = "pink";
    color_map[5] = "orange";

    std::string dot_str = "digraph G {\n";
    dot_str += "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    
    dot_str += std::to_string(init_state) + " [fillcolor=green]\n"; //Make the initial state a different color 

    for (auto state : hashStorage.get_state_vec()) {
        std::string action_str = ""; //build a string of all the actions and total Samples of state 
        int color_ctr = 0; //increment the state color per action 
        for (auto action : hashStorage.get_state_actions_vec(state)) {
            action_str += "\\n act: " + std::to_string(action) 
            + " | #tot_spl: " + std::to_string(hashStorage.get_total_samples(state, action));
            for (auto succ : hashStorage.get_state_action_succ_vec(state, action)) {
                dot_str += "  " + std::to_string(state) + " -> " + std::to_string(succ) // transition 
                + " [label=\"act: "  + std::to_string(action) + "\\n #spl: " //label with action and samples 
                + std::to_string(hashStorage.get_succ_samples(state, action, succ)) 
                + "\", color=" + color_map[color_ctr] + "]\n"; //color of trabsition 
            }
            color_ctr++;
        }
        dot_str += "  " + std::to_string(state) + " [ label=\"state: " + std::to_string(state) + action_str + "\"]\n"; // text in state 
    }

    dot_str += "}\n";
    return dot_str;
}

template<typename StateType>
void eMDP<StateType>::writeDotFile(std::string filename) {
    std::ofstream MyFile(filename);
    MyFile << toDotString();
    MyFile.close();
}

template<typename StateType>
void eMDP<StateType>::printDot() {
    std::cout(toDotString());
}

template<typename StateType>
void eMDP<StateType>::addVisit(StateType state, StateType action, StateType succ) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, 1);
}

template<typename StateType>
void eMDP<StateType>::addVisits(StateType state, StateType action, StateType succ, StateType visits) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, visits);
}

template<typename StateType>
void eMDP<StateType>::addState(StateType state, std::vector<StateType> avail_actions) {
    addStateToExplorationOrder(state);
    hashStorage.add_state_actions(state, avail_actions);
}

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
    return stateLabeling[state];
}

template<typename StateType>
bool eMDP<StateType>::isStateKnown(StateType state) {
    return hashStorage.state_exists(state);
}

template<typename StateType>
StateType eMDP<StateType>::getSampleCount(StateType state, StateType action) {
    return hashStorage.get_total_samples(state, action);
}

template<typename StateType>
StateType eMDP<StateType>::getSampleCount(StateType state, StateType action, StateType succ) {
    return hashStorage.get_succ_samples(state, action, succ);
}

template<typename StateType>
void eMDP<StateType>::setSuccCount(StateType state, StateType action, StateType count) {
    hashStorage.set_succ_count(std::make_pair(state, action), count);
}

template<typename StateType>
StateType eMDP<StateType>::getSuccCount(StateType state, StateType action) {
    return hashStorage.get_succ_count(std::make_pair(state, action));
}

template<typename StateType> 
StateType eMDP<StateType>::getActionCount(StateType state) {
    return hashStorage.get_action_count(state);
}

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::get_state_itr() {
    return hashStorage.get_state_itr();
}

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::get_state_actions_itr(StateType state) {
    return hashStorage.get_state_actions_itr(state);
}

template<typename StateType>
storage::KeyIterator<StateType> eMDP<StateType>::get_state_action_succ_itr(StateType state, StateType action) {
    return hashStorage.get_state_action_succ_itr(state, action);
}

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

