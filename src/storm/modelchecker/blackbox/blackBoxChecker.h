//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BLACKBOXCHECKER_H
#define STORM_BLACKBOXCHECKER_H

#include "storm/modelchecker/AbstractModelChecker.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType = uint32_t>
class blackBoxChecker: public storm::modelchecker::AbstractModelChecker<Modeltype> {
    public:
     blackBoxChecker(storm::prism::Program const& program);
     
     virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;

     void buildAndPrintEmdp();
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXCHECKER_H
