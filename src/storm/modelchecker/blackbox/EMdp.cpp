#include <algorithm>

#include "EMdp.h"

#include <algorithm>

#include <regex>

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
EMdp<StateType>::EMdp() : hashStorage(), stateLabeling() {
}

template<typename StateType>
std::string EMdp<StateType>::labelVecToStr(std::vector<std::string> labelVec) {
    std::string result =  "[";
    for(auto label : labelVec) 
        result += label + ", ";
    if(!labelVec.empty()) {
        result.pop_back();
        result.pop_back();
    }
    return result + "]";
}

template<typename StateType>
void EMdp<StateType>::eMdpToFile(std::string fileName) {
      std::ofstream MyFile(fileName);

      
      MyFile << "init: " << initState << "\n";


      for(auto stateItr = getStateItr(); stateItr.hasNext();) {
        auto state = stateItr.next();
        MyFile << state << ": " << labelVecToStr(getStateLabels(state)) << "\n";

        for(auto actItr = getStateActionsItr(state); actItr.hasNext();) {
            auto action = actItr.next();
            MyFile << "\t" << action << ": " << labelVecToStr(getActionLabels(state, action)) << "\n";

            for(auto succItr = getStateActionsSuccItr(state, action); succItr.hasNext();) {
                auto succ = succItr.next();
                MyFile << "\t\t" << succ << ": " << getSampleCount(state, action) << "\n";
            }
        }
      }
      MyFile << "eof";
      MyFile.close();
}

template<typename StateType>
EMdp<StateType> EMdp<StateType>::eMdpFromFile(std::string fileName) {
    auto newEMdp = EMdp<StateType>();
    std::ifstream myFile(fileName);
    std::string line;
    std::regex init("init: [[:digit:]]+");

    std::regex stateRe("[[:digit:]]+: \\[(([^,]+, )*[^,]+)?\\]"); //matches expr of form "state: [act1, act2]""
    std::regex actionRe("\\t[[:digit:]]+: \\[(([^,]+, )*[^,]+)?\\]"); //matches expr of form "\tstate:[act1, act2]"
    std::regex succRe("\\t\\t[[:digit:]]+: [[:digit:]]+"); //matches expr of form "\t\tstate: count"

    std::regex e("([a-zA-Z]|[[:digit:]])+");


    std::getline(myFile, line);

    if(std::regex_match(line, init)) { 
        std::regex s("[[:digit:]]+");
        std::sregex_iterator iter(line.begin(), line.end(), s);
        newEMdp.addInitialState(stoi(iter->str()));
    } else { 
        std::cout << "Wrong file format!\n";
        return EMdp();
    } 

    bool lastLineWasState = false;  
    StateType lastState = -1;
    StateType lastAction = -1;

    while(!myFile.eof()) {
        std::getline(myFile, line);
        std::sregex_iterator iter(line.begin(), line.end(), e);
        std::sregex_iterator rend;

        if (line.compare("eof") == 0) {
            break;
        } else if(std::regex_match(line, stateRe)) {
            lastLineWasState = true;
            lastState = stoi(iter->str());
            iter++;

            while (iter!=rend) {
                newEMdp.addStateLabel(iter->str(), lastState);
                ++iter;
            }
        } 
        else if(std::regex_match(line, actionRe)) {
            lastLineWasState = false;
            lastAction = stoi(iter->str());
            iter++;

            while (iter!=rend) {
                newEMdp.addActionLabel(iter->str(), lastState, lastAction);
                ++iter;
            }
        } 
        else if(std::regex_match(line, succRe) && !lastLineWasState) {
            lastLineWasState = false;
            //std::cout << stoi((*iter)[0]) << ": " << stoi((*iter)[1]) << std::endl; //TODO
            newEMdp.addVisits(lastState, lastAction, stoi((*iter)[0]), stoi((*iter)[1]));
        } 
        else {
            std::cout << "Wrong file format!\n";
            return EMdp();
        }
    }

    return newEMdp;
}


template<typename StateType>
void EMdp<StateType>::print() {
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
StateType EMdp<StateType>::getInitialState() {
    return initState;
}

template<typename StateType>
void EMdp<StateType>::addState(StateType state, std::vector<StateType> availActions) {
    hashStorage.addStateActions(state, availActions);
}

template<typename StateType>
void EMdp<StateType>::addVisit(StateType state, StateType action, StateType succ) {
    hashStorage.incTrans(state, action, succ, 1);
}

template<typename StateType>
void EMdp<StateType>::addVisits(StateType state, StateType action, StateType succ, StateType visits) {
    hashStorage.incTrans(state, action, succ, visits);
}

template<typename StateType>
void EMdp<StateType>::addUnsampledAction(StateType state, StateType action) {
    hashStorage.addUnsampledAction(state, action);
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


int main(int argc, char const *argv[]) {
    
    auto emdp = storm::modelchecker::blackbox::EMdp<int_fast32_t>();
    emdp.addInitialState(1);
    
    emdp.addStateLabel("label1", 0);
    emdp.addStateLabel("label2", 0);
    emdp.addStateLabel("label1", 1);
    emdp.addStateLabel("label2", 1);
    emdp.addStateLabel("label1", 2);
    emdp.addStateLabel("label2", 2);

    emdp.addActionLabel("actLabel", 0, 5);
    emdp.addActionLabel("actLabel", 0, 6);

    emdp.addVisits(0,5,1,2);
    emdp.addVisits(0,6,2,3);
    
    emdp.eMdpToFile("emdp_test.txt");
    auto x = emdp.eMdpFromFile("emdp_test.txt");
    x.print();
}




