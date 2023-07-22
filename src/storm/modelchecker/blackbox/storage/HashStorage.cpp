#include "HashStorage.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

//-------------------- Iterator Methods ---------------------------//

template<typename StateType>
KeyIterator<StateType>::KeyIterator(void* map) {
    cur = (*(std::unordered_map<int, void*>*) map).begin();
    end = (*(std::unordered_map<int, void*>*) map).end();
}

template<typename StateType>
KeyIterator<StateType>::KeyIterator() {
    std::unordered_map<int, void*> emptyMap;
    cur = emptyMap.begin();
    end = emptyMap.end();
}

template<typename StateType>
StateType KeyIterator<StateType>::next() {
    StateType key = cur->first;
    cur++;
    return key;
}

template<typename StateType>
StateType KeyIterator<StateType>::peek() {
    return cur->first;
}

template<typename StateType>
bool KeyIterator<StateType>::hasNext() {
    return cur != end;
} 
//--------------------- HashStorage Methods -------------------------//

template<typename StateType>
HashStorage<StateType>::HashStorage() : data() {
}

template<typename StateType>
std::unordered_map<StateType, StateType> HashStorage<StateType>::getSuccMap(StateType state, StateType action) {
    std::unordered_map<StateType, StateType> succMap;
    if (data.find(state) == data.end()) {
        return succMap;
    }

    auto actMap = &data.at(state);
    if (actMap->find(action) == actMap->end()) {
        return succMap;
    }

    succMap = actMap->at(action).second;
    return succMap;
}

template<typename StateType>
void HashStorage<StateType>::addState(StateType state) {
    if (data.find(state) == data.end()) {
           totalStateCount++;
           data[state] = std::unordered_map<StateType, countSampleMapPair>();
    }
}

//__________________ Add states and actions to Datastructure __________//
template<typename StateType>
void HashStorage<StateType>::addStateActions(StateType state, std::vector<StateType> actions) {
    addState(state);
    auto* actMap = &data.at(state);
    if (actMap->begin() == actMap->end()) {
        for (auto act : actions) {
            if (actMap->find(act) == actMap->end()) {
                totalStateActionPairCount++;
                (*actMap)[act] = countSampleMapPair();
            }
        }
    }
}

template<typename StateType>
void HashStorage<StateType>::addUnsampledAction(StateType state, StateType action) {
    addState(state);  // add state to data if it doesn't exist

    auto* actMap = &data.at(state);  // add action to data if it doesn't exist
    if (actMap->find(action) == actMap->end()) {
        totalStateActionPairCount++;
        (*actMap)[action] = countSampleMapPair();
    }
}


template<typename StateType>
void HashStorage<StateType>::incTrans(StateType state, StateType action, StateType succ, StateType samples) {
    addState(state);  // add state to data if it doesn't exist
    addState(succ);   // add succ to data if it doesn't exist

    auto* actMap = &data.at(state);  // add action to data if it doesn't exist
    if (actMap->find(action) == actMap->end()) {
        totalStateActionPairCount++;
        (*actMap)[action] = countSampleMapPair();
    }

    auto* sampleMap = &(actMap->at(action).second);
    (*actMap).at(action).first += samples;  // Increment the total samples for the action

    if(sampleMap->find(succ) == sampleMap->end())
        totalTransitionCount++;
    (*sampleMap)[succ] += samples;          // Increments the samples for the (state,action,succ) triple
}

//__________________ Access the Datastructure via Vectors _________________//

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::getStateVec() {
    std::vector<StateType> stateVec;
    for (auto const& p : data) stateVec.push_back(p.first);
    return stateVec;
}

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::getStateActionVec(StateType state) {
    std::vector<StateType> actionVec;
    if (data.find(state) != data.end())
        for (auto const& p : data.at(state)) actionVec.push_back(p.first);
    return actionVec;
}

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::getStateActionSuccVec(StateType state, StateType action) {
    std::vector<StateType> succVec;
    for (auto const& p : getSuccMap(state, action)) succVec.push_back(p.first);
    return succVec;
}
//__________________ Access the Datastructure via Iterators _________________//

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::getStateItr() {
    return KeyIterator<StateType>(&data);
}

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::getStateActionsItr(StateType state) {
    if (data.find(state) == data.end()) {
        return KeyIterator<StateType>();
    }
    return KeyIterator<StateType>(&data.at(state));
}

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::getStateActionsSuccItr(StateType state, StateType action) {
    if (data.find(state) == data.end()) {
        return KeyIterator<StateType>();
    }
    auto* actMap = &data.at(state);
    if(actMap->find(action) == actMap->end()) {
        return KeyIterator<StateType>();
    }
    return KeyIterator<StateType>(&(actMap->at(action).second));
}
//_______________________________ Access predeccesors of states ___________________________//
template<typename StateType> 
void HashStorage<StateType>::createReverseMapping() {
    reverseMap = std::unordered_map<StateType, std::vector<std::pair<StateType, StateType> > >(); // delete previous map 

    auto stateItr = getStateItr();
    while (stateItr.hasNext()) {
        StateType predState = stateItr.next();
        auto actionItr = getStateActionsItr(predState);
        while (actionItr.hasNext()) {
            StateType action = actionItr.next();
            auto succItr = getStateActionsSuccItr(predState, action);
            while(succItr.hasNext()) {
                StateType succ = succItr.next();
                reverseMap[succ].push_back(std::make_pair(predState, action));
            }
        }
    }
}

template<typename StateType>
std::vector<std::pair<StateType, StateType>> HashStorage<StateType>::getPredecessors(StateType state) {
    if(reverseMap.find(state) != reverseMap.end())
        return reverseMap[state];
    return std::vector<std::pair<StateType, StateType>>(); 
}

//__________________ Access seperate elements of the Datastructure _____________________//

template<typename StateType>
bool HashStorage<StateType>::stateExists(StateType state) {
    return data.find(state) != data.end();
}

template<typename StateType>
StateType HashStorage<StateType>::getTotalStateCount() {
    return totalStateCount;
}

template<typename StateType>
StateType HashStorage<StateType>::gettotalStateActionPairCount() {
    return totalStateActionPairCount;
}

template<typename StateType>
StateType HashStorage<StateType>::getTotalTransitionCount() {
    return totalTransitionCount;
}

template<typename StateType>
StateType HashStorage<StateType>::getTotalSamples(StateType state, StateType action) {
    if (data.find(state) == data.end()) {
        return -1;
    }

    auto* actMap = &data.at(state);
    if (actMap->find(action) == actMap->end()) {
        return -1;
    }
    return actMap->at(action).first;
}

template<typename StateType>
StateType HashStorage<StateType>::getSuccSamples(StateType state, StateType action, StateType succ) {
    auto succMap = getSuccMap(state, action);

    if (succMap.find(succ) != succMap.end())
        return succMap.at(succ);
    return -1;
}

template<typename StateType>
void HashStorage<StateType>::setSuccCount(std::pair<StateType, StateType> stateActionPair, StateType count) {
    succCountMap[stateActionPair] = count;
}

template<typename StateType>
StateType HashStorage<StateType>::getSuccCount(std::pair<StateType, StateType> stateActionPair) {
    if(succCountMap.find(stateActionPair) != succCountMap.end())
        return succCountMap[stateActionPair];
    return -1;
}

template<typename StateType>
void HashStorage<StateType>::print() {
    std::cout << "Total state count: " << getTotalStateCount() << std::endl;
    std::cout << "Total state action pair count: " << gettotalStateActionPairCount() << std::endl;
    std::cout << "Total transition count: " << getTotalTransitionCount() << std::endl;
    for (auto state : getStateVec()) {
        std::cout << "-------------------------\n";
        std::cout << "State: " << state << "\n";
        for (auto action : getStateActionVec(state)) {
            std::cout << " Action: " << action << " | Total Samples: " << getTotalSamples(state, action) << "\n";
            for (auto succ : getStateActionSuccVec(state, action)) {
                std::cout << "  * Succ: " << succ << " | Samples: " << getSuccSamples(state, action, succ) << "\n";
            }
        }
    }
}

template class HashStorage<uint32_t>; //Type for which class gets compiled
template class HashStorage<uint64_t>;
template class KeyIterator<uint32_t>;
template class KeyIterator<uint64_t>;
}
}
}
}

/*
int main(int argc, char const *argv[])
{       
    
    auto x = storm::modelchecker::blackbox::storage::HashStorage<int_fast32_t>();
    x.incTrans(1,0,3,0);
    x.incTrans(1,0,4,0);
    x.incTrans(1,0,5,0);
    x.incTrans(1,1,6,0);
    x.incTrans(1,0,5,3);
    x.print();
}
*/





