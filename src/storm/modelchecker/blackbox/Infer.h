#pragma once
#include <utility>
#include "BlackboxInterface.h"
#include "modelchecker/blackbox/BMdp.h"
#include "modelchecker/blackbox/EMdp.h"
#include "modelchecker/blackbox/boundFunctions/BoundFunc.h"
#include "modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"

using storm::modelchecker::blackbox::EMdp;
using storm::models::sparse::BMdp;
template <typename IndexType, typename ValueType, typename StateType>
BMdp<ValueType> infer(EMdp<IndexType> &emdp, BoundFunc<ValueType> &boundFunc, DeltaDistribution<IndexType> &valueFunc, double pmin, double delta, bool isGreybox, std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType, ValueType>>);

