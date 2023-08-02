#include "storm/modelchecker/blackbox/EMdpToDot.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
EMdpDotGenerator<StateType>::EMdpDotGenerator(bool incAction, bool incSamples, bool incLabel, bool incColor) {
    includeAction = incAction; 
    includeSamples = incSamples;
    includeLabel = incLabel;
    includeColor = incColor;

    colorMap[0] = "red";
    colorMap[1] = "blue";
    colorMap[2] = "green";
    colorMap[3] = "orange";
    colorMap[4] = "pink";
    colorMap[5] = "brown";
    colorMap[6] = "black";
};



template<typename StateType>
std::string EMdpDotGenerator<StateType>::colorObj(StateType colorCtr) {
    return "color=\"" + colorMap[std::clamp(colorCtr, StateType(0), StateType(6))] + "\"";
}

template<typename StateType>
std::string EMdpDotGenerator<StateType>::colorObj(std::string color) {
    return "color=\"" + color + "\"";
}

template<typename StateType>
std::string EMdpDotGenerator<StateType>::addLabels(std::vector<std::string> labelVec) {
    std::string result =  "labels: [";
    for(auto label : labelVec) 
        result += label + ", ";
    if(!labelVec.empty()) {
        result.pop_back();
        result.pop_back();
    }
    return result + "]";
}

template<typename StateType>
void EMdpDotGenerator<StateType>::addTrans(StateType state1, StateType state2, std::ostream& outStream) {
    outStream << state1 << " -> " << state2 << " ";
}

template<typename StateType> 
void EMdpDotGenerator<StateType>::addEmdpStateDotLabel(StateType state, std::vector<std::string> labelVec, std::string color, std::ostream& outStream) {
    outStream << state << " [label=\"name: " << state;
    outStream << (includeLabel ? "\\n" + addLabels(labelVec) : "") << "\"";
    outStream << (includeColor ? ", " + colorObj(color) : "");
    outStream << "]" << std::endl;
}

template<typename StateType>
void EMdpDotGenerator<StateType>::addEmdpDotLabel(StateType action, StateType samples, std::vector<std::string> labelVec, StateType colorCtr, std::ostream& outStream) {
    outStream << "[label=\"";
    outStream << (includeAction  ? "act: " + std::to_string(action) + "\\n" : "");
    outStream << (includeSamples ? "#samples: " + std::to_string(samples) + "\\n" : "");
    outStream << (includeLabel   ? addLabels(labelVec) + "\\n" : "") << "\"";
    outStream << (includeColor   ? ", " + colorObj(colorCtr) : "");
    outStream << "]" << std::endl;
}

template<typename StateType> 
void EMdpDotGenerator<StateType>::convertPred(EMdp<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited) {
    addEmdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
    if(depth > 0) {
        for(auto pred_pair: emdp.getPredecessors(state)) {
            StateType pred = pred_pair.first;
            StateType action = pred_pair.second;

            if(std::find(visited->begin(), visited->end(), std::make_tuple(pred, action, state)) == visited->end()) {
                addTrans(pred, state, outStream);
                addEmdpDotLabel(action, emdp.getSampleCount(pred, action, state), emdp.getActionLabels(state, action), 0, outStream);
                visited->push_back(std::make_tuple(pred, action, state));
                convertPred(emdp, pred, depth - 1, outStream, visited);
                convertSucc(emdp, pred, depth - 1, outStream, visited);
            }
        }
    }

}

template<typename StateType>
void EMdpDotGenerator<StateType>::convertSucc(EMdp<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited) {
    addEmdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
    if(depth > 0) {
        auto actionItr = emdp.getStateActionsItr(state);
        while (actionItr.hasNext())
        {   
            auto succItr = emdp.getStateActionsSuccItr(state, actionItr.peek());
            StateType action = actionItr.peek();
            while (succItr.hasNext()) {
                StateType succ = succItr.next();
                if(std::find(visited->begin(), visited->end(), std::make_tuple(state, action, succ)) == visited->end()) {
                    addTrans(state, succ, outStream);
                    addEmdpDotLabel(action, emdp.getSampleCount(state, action, succ), emdp.getActionLabels(state, action), 0, outStream);
                    visited->push_back(std::make_tuple(state, action, succ));
                    convertSucc(emdp, succ, depth - 1, outStream, visited);
                }    
            }
            actionItr.next();
        } 
    }
}

template<typename StateType> 
void EMdpDotGenerator<StateType>::convertNeighborhood(EMdp<StateType> emdp, StateType state, StateType depth, std::ostream& outStream) {
    outStream << "digraph G {\n";
    outStream << "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    std::vector<std::tuple<StateType, StateType, StateType>> visited;
    convertPred(emdp, state, depth, outStream, &visited);
    convertSucc(emdp, state, depth, outStream, &visited);
    addEmdpStateDotLabel(state, emdp.getStateLabels(state), "green", outStream);
    outStream << "}\n";
}

template<typename StateType> 
void EMdpDotGenerator<StateType>::convert(EMdp<StateType> emdp, std::ostream& outStream) {
    outStream << "digraph G {\n";
    outStream << "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    auto stateItr = emdp.getStateItr();
    while (stateItr.hasNext())
    {   
        StateType state = stateItr.next();
        addEmdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
        auto actionItr = emdp.getStateActionsItr(state);
        StateType actColCtr = 0;
        while (actionItr.hasNext())
        {   
            StateType action = actionItr.next();
            auto succItr = emdp.getStateActionsSuccItr(state, action);
                while (succItr.hasNext()) {
                    StateType succ = succItr.next();
                    addTrans(state, succ, outStream);
                    addEmdpDotLabel(action, emdp.getSampleCount(state, action, succ), emdp.getActionLabels(state, action), actColCtr, outStream);
                }
            actColCtr++;
        }
        
    }
    addEmdpStateDotLabel(emdp.getInitialState(), emdp.getStateLabels(emdp.getInitialState()), "green", outStream);
    outStream << "}\n";
}

template class EMdpDotGenerator<uint32_t>;
template class EMdpDotGenerator<uint64_t>;

}
}
}

