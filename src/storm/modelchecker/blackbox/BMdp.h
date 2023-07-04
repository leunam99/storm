#ifndef STORM_Bmdp_H
#define STORM_Bmdp_H

#include "models/sparse/NondeterministicModel.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

namespace storm {
namespace models {
namespace sparse {

/*!
* Template class to store the bounds of the BMdp
* ValueType of BMdp
*/
template<typename ValueType>
class ValueTypePair {
   private:
    std::pair<ValueType,ValueType> valuePair;
   public:

    ValueTypePair(std::pair<ValueType,ValueType> valuePair): valuePair{valuePair}{}
    ValueTypePair(std::pair<ValueType,ValueType> &&valuePair): valuePair{std::move(valuePair)}{}


    ValueType getLBound() const  {
        valuePair.first();
    }
    ValueType getUBound() const {
        valuePair.second();
    }
    std::pair<ValueType,ValueType> operator+(ValueTypePair other) {
        return std::make_pair(getLBound() + other.getLBound(), getUBound() + other.getUBound());
    }
};

/**
 * Prints the value pair to the stream
 * @return a reference to the stream
 */
template <class T>
std::ostream& operator<<(std::ostream &os, const ValueTypePair<T>& vp){
    os << "[" << vp.getLBound() << "," << vp.getUBound() << "]";
    return os;
}

template<class BoundType, typename RewardModelType = StandardRewardModel<BoundType>>
class BMdp : public NondeterministicModel<ValueTypePair<BoundType>, RewardModelType> {

    using BoundPair = ValueTypePair<BoundType>;

    public:
     /*!
     * Constructs a model from the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
     BMdp(storm::storage::SparseMatrix<BoundPair> const& transitionMatrix, StateLabeling const& stateLabeling,
        std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
        ModelType type = ModelType::BMdp);

     /*!
     * Constructs a model by moving the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
     BMdp(storm::storage::SparseMatrix<BoundPair>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
        std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(), ModelType type = ModelType::BMdp);

    /*!
     * Constructs a model from the given data.
     *
     * @param components The components for this model.
     */
     BMdp(storm::storage::sparse::ModelComponents<BoundPair, RewardModelType> const& components, ModelType type = ModelType::BMdp);
     BMdp(storm::storage::sparse::ModelComponents<BoundPair, RewardModelType>&& components, ModelType type = ModelType::BMdp);

     BMdp(BMdp<BoundType, RewardModelType> const& other) = default;
     BMdp& operator=(BMdp<BoundType, RewardModelType> const& other) = default;

     BMdp(BMdp<BoundType, RewardModelType>&& other) = default;
     BMdp& operator=(BMdp<BoundType, RewardModelType>&& other) = default;
    virtual ~BMdp() = default;
};

}  // namespace sparse
}  // namespace models
}  // namespace storm

#endif  // STORM_Bmdp_H

