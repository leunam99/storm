#include "EMdpToDot.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
EMdpDotGenerator<StateType>::EMdpDotGenerator(bool includeAction, bool includeSamples, bool includeLabel, bool includeColor) {
    includeAction = includeAction; 
    includeSamples = includeSamples;
    includeLabel = includeLabel;
    includeColor = includeColor;


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
void EMdpDotGenerator<StateType>::addEMdpStateDotLabel(StateType state, std::vector<std::string> labelVec, std::string color, std::ostream& outStream) {
    outStream << state << " [label=\"name: " << state;
    outStream << (includeLabel ? "\\n" + addLabels(labelVec) : "");
    outStream << (includeColor ? "\", " + colorObj(color) : "");
    outStream << "]" << std::endl;
}

template<typename StateType>
void EMdpDotGenerator<StateType>::addEMdpDotLabel(StateType action, StateType samples, std::vector<std::string> labelVec, StateType colorCtr, std::ostream& outStream) {
    outStream << "[label=\"";
    outStream << (includeAction  ? "act: " + std::to_string(action) + "\\n" : "");
    outStream << (includeSamples ? "#samples: " + std::to_string(samples) + "\\n" : "");
    outStream << (includeLabel   ? addLabels(labelVec) + "\\n" : "");
    outStream << (includeColor   ? "\", " + colorObj(colorCtr) : "");
    outStream << "]" << std::endl;
}

template<typename StateType> 
void EMdpDotGenerator<StateType>::convertPred(EMdp<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited) {
    addEMdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
    if(depth > 0) {
        for(auto pred_pair: emdp.getPredecessors(state)) {
            StateType pred = pred_pair.first;
            StateType action = pred_pair.second;

            if(std::find(visited->begin(), visited->end(), std::make_tuple(pred, action, state)) == visited->end()) {
                addTrans(pred, state, outStream);
                addEMdpDotLabel(action, emdp.getSampleCount(pred, action, state), emdp.getActionLabels(state, action), 0, outStream);
                visited->push_back(std::make_tuple(pred, action, state));
                convertPred(emdp, pred, depth - 1, outStream, visited);
                convertSucc(emdp, pred, depth - 1, outStream, visited);
            }
        }
    }

}

template<typename StateType>
void EMdpDotGenerator<StateType>::convertSucc(EMdp<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited) {
    addEMdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
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
                    addEMdpDotLabel(action, emdp.getSampleCount(state, action, succ), emdp.getActionLabels(state, action), 0, outStream);
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
    addEMdpStateDotLabel(state, emdp.getStateLabels(state), "green", outStream);
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
        addEMdpStateDotLabel(state, emdp.getStateLabels(state), "grey", outStream);
        auto actionItr = emdp.getStateActionsItr(state);
        StateType actColCtr = 0;
        while (actionItr.hasNext())
        {   
            StateType action = actionItr.next();
            auto succItr = emdp.getStateActionsSuccItr(state, action);
                while (succItr.hasNext()) {
                    StateType succ = succItr.next();
                    addTrans(state, succ, outStream);
                    addEMdpDotLabel(action, emdp.getSampleCount(state, action, succ), emdp.getActionLabels(state, action), actColCtr, outStream);
                }
            actColCtr++;
        }
        
    }
    addEMdpStateDotLabel(emdp.getInitialState(), emdp.getStateLabels(emdp.getInitialState()), "green", outStream);
    outStream << "}\n";
}

}
}
}


int main(int argc, char const *argv[])
{   

    auto emdp = storm::modelchecker::blackbox::EMdp<int_fast32_t>();
    emdp.addVisit(0,0,1);
    emdp.addVisit(0,0,7);

    emdp.addVisit(1,0,2);
    emdp.addVisit(1,0,8);
    emdp.addVisit(1,0,9);

    emdp.addVisit(30, 0, 1);
    emdp.addVisit(30, 1, 1);
    emdp.addVisit(30, 2, 1);
    emdp.addVisit(34, 1, 1);

    emdp.addVisit(2,0,3);
    emdp.addVisit(3,0,10);

    emdp.addActionLabel("Label1", 0,0);
    emdp.addActionLabel("Label2",0,0);

    emdp.addStateLabel("StateL", 0);
    

    emdp.createReverseMapping();


    auto dot = storm::modelchecker::blackbox::EMdpDotGenerator<int_fast32_t>(true,true,true,true);
    dot.convertNeighborhood(emdp, 1, 2, std::cout);
    std::cout << "Helooooooo";
    return 0;
    
}

