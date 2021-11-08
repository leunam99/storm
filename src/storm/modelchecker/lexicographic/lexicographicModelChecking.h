//
// Created by steffi on 04.11.21.
//
#pragma once

#include <memory>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {

            template<typename SparseModelType, typename ValueType>
            int isDone(Environment const& env, SparseModelType const& model,  CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask);

        }
    }
}