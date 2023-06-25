#include "eMDPtoDot.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
eMDPDotGenerator<StateType>::eMDPDotGenerator() {};

template<typename StateType>
eMDPDotGenerator<StateType>::eMDPDotGenerator(bool include_action, bool include_samples, bool include_label, bool include_color) {
    include_action = include_action; 
    include_samples = include_samples;
    include_label = include_label;
    include_color = include_color;
};

template<typename StateType>
std::string eMDPDotGenerator<StateType>::color_obj(StateType color_ctr) {

    return "color=\"red\"";
}

template<typename StateType>
std::string eMDPDotGenerator<StateType>::color_obj(std::string color) {


    return "color=\"" + color + "\"";
}

template<typename StateType>
std::string eMDPDotGenerator<StateType>::add_labels(std::vector<std::string> label_vec) {
    std::string result =  "labels: [";
    for(auto label : label_vec) 
        result += label + ", ";
    if(!label_vec.empty()) {
        result.pop_back();
        result.pop_back();
    }
    return result + "]";
}

template<typename StateType>
void eMDPDotGenerator<StateType>::add_trans(StateType state1, StateType state2, std::ostream& outStream) {
    outStream << state1 << " -> " << state2 << " ";
}

template<typename StateType> 
void eMDPDotGenerator<StateType>::emdp_state_dotLabel(StateType state, std::vector<std::string> label_vec, std::string color, std::ostream& outStream) {
    outStream << state << " [label=\"name: " << state;
    outStream << (include_label ? "\\n" + add_labels(label_vec) : "");
    outStream << (include_color ? "\", " + color_obj(color) : "");
    outStream << "]" << std::endl;
}

template<typename StateType>
void eMDPDotGenerator<StateType>::emdp_trans_dotLabel(StateType action, StateType samples, std::vector<std::string> label_vec, StateType color_ctr, std::ostream& outStream) {
    outStream << "[label=\"";
    outStream << (include_action  ? "act: " + std::to_string(action) + "\\n" : "");
    outStream << (include_samples ? "#samples: " + std::to_string(samples) + "\\n" : "");
    outStream << (include_label   ? add_labels(label_vec) + "\\n" : "");
    outStream << (include_color   ? "\", " + color_obj(color_ctr) : "");
    outStream << "]" << std::endl;
}

template<typename StateType> 
void eMDPDotGenerator<StateType>::convert_pred(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>> visited) {
    emdp_state_dotLabel(state, emdp.getStateLabels(state), "grey", outStream);
    if(depth > 0) {
        for(auto pred_pair: emdp.get_predecessors(state)) {
            if(std::find(visited.begin(), visited.end(), std::make_tuple(pred_pair.first, pred_pair.second, state)) == visited.end()) {
                visited.push_back(std::make_tuple(pred_pair.first, pred_pair.second, state));
                add_trans(pred_pair.first, state, outStream);
                convert_pred(emdp, pred_pair.first, depth - 1, outStream, visited);
                convert_succ(emdp, pred_pair.first, depth - 1, outStream, visited);
            }
        }
    }
}

template<typename StateType> 
void eMDPDotGenerator<StateType>::convert_succ(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>> visited) {
    emdp_state_dotLabel(state, emdp.getStateLabels(state), "grey", outStream);
    if(depth > 0) {
        auto action_itr = emdp.get_state_actions_itr(state);
        while (action_itr.hasNext())
        {   
            auto succ_itr = emdp.get_state_action_succ_itr(state, action_itr.peek());
            StateType action = action_itr.peek();
            while (succ_itr.hasNext()) {
                StateType succ = succ_itr.next();
                if(std::find(visited.begin(), visited.end(), std::make_tuple(state, action, succ)) == visited.end()) {
                    visited.push_back(std::make_tuple(state, action, succ));
                    add_trans(state, succ, outStream);
                    convert_succ(emdp, succ, depth - 1, outStream, visited);
                    convert_pred(emdp, succ, depth - 1, outStream, visited);
                }    
            }
            action_itr.next();
        } 
    }
}

template<typename StateType> 
void eMDPDotGenerator<StateType>::convert_neighborhood_eMDP(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream) {
    outStream << "digraph G {\n";
    outStream << "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    std::vector<std::tuple<StateType, StateType, StateType>> visited;
    convert_pred(emdp, state, depth, outStream, visited);
    convert_succ(emdp, state, depth, outStream, visited);
    outStream << "}\n";
}

template<typename StateType> 
void eMDPDotGenerator<StateType>::convert_eMDP(eMDP<StateType> emdp, std::ostream& outStream) {
    outStream << "digraph G {\n";
    outStream << "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    auto state_itr = emdp.get_state_itr();
    while (state_itr.hasNext())
    {   
        StateType state = state_itr.next();
        emdp_state_dotLabel(state, emdp.getStateLabels(state), "grey", outStream);
        auto action_itr = emdp.get_state_actions_itr(state);
        while (action_itr.hasNext())
        {   
            StateType act_col_ctr = 1;
            StateType action = action_itr.next();
            auto succ_itr = emdp.get_state_action_succ_itr(state, action);
                while (succ_itr.hasNext()) {
                    StateType succ = succ_itr.next();
                    add_trans(state, succ, outStream);
                    emdp_trans_dotLabel(action, emdp.getSampleCount(state, action, succ), emdp.getActionLabels(state, action), act_col_ctr, outStream);
                }
        }
        
    }
    outStream << "}\n";
}

}
}
}

int main(int argc, char const *argv[])
{   

    auto emdp = storm::modelchecker::blackbox::eMDP<int_fast32_t>();
    emdp.addVisit(0,0,1);
    emdp.addVisit(0,0,7);

    emdp.addVisit(1,0,2);
    emdp.addVisit(1,0,8);
    emdp.addVisit(1,0,9);

    emdp.addVisit(2,0,3);
    emdp.addVisit(3,0,10);

    emdp.addActionLabel("Label1", 0,0);
    emdp.addActionLabel("Label2",0,0);

    emdp.addStateLabel("StateL", 0);
    

    emdp.createReverseMapping();


    auto dot = storm::modelchecker::blackbox::eMDPDotGenerator<int_fast32_t>(false,false,false,false);;
    dot.convert_neighborhood_eMDP(emdp, 2, 3, std::cout);

    return 0;
    
}





/*
template<typename StateType>
void toDotString() {
    

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
*/