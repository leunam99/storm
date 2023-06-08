//
// Created by Maximilian Kamps on 25.05.23.
//

#include "blackBoxChecker.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
blackboxChecker<ModelType, StateType>::blackBoxChecker(storm::prism::Program const& program) {}

template<typename ModelType, typename StateType>
bool blackboxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override {
    return false;
}

template<typename ModelType, typename StateType>
void blackboxChecker<ModelType, StateType>::buildAndPrintEmdp() {
    
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
