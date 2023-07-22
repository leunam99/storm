#ifndef STORM_EMDP2BMDP_H
#define STORM_EMDP2BMDP_H

#include <utility>
#include "modelchecker/blackbox/BMdp.h"
#include "modelchecker/blackbox/EMdp.h"
#include "modelchecker/blackbox/bound-functions/BoundFunc.h"
#include "modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"

using storm::models::sparse::BMdp;
using storm::modelchecker::blackbox::EMdp;
template <typename IndexType, typename ValueType>
BMdp<ValueType> infer(EMdp<IndexType> &emdp, BoundFunc<ValueType> &boundFunc, DeltaDistribution<IndexType> &valueFunc, double pmin, double delta, bool isBlackbox);

#endif  // STORM_EMDP2BMDP_H
