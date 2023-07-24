#ifndef STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_
#define STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_

#include <random>

#include "storm/modelchecker/blackbox/bound-functions/BoundFunc.h"
#include "storm/modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

// TODO create methods to retrieve infer options

/*!
 * This class represents the blackbox settings.
 */
class BlackboxSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of exploration settings.
     */
    BlackboxSettings();

    /*!
     * Retrieves the filename the eMDP should be written too.
     *
     * @return the filename.
     */
    std::string getEMdpOutFileName() const;

    /*!
     * Retrieves the filename where the eMDP for dot conversion is stored
     *
     * @return the filename.
     */
    std::string getEMdpDotInFileName() const;

    /*!
     * Retrieves the filename the dot string should be written too.
     *
     * @return the filename.
     */
    std::string getEMdpDotOutFileName() const;

    /*!
     * Retrieves the filename where the eMDP for neighborhood dot conversion is stored
     *
     * @return the filename.
     */
    std::string getEMdpNeighborhoodDotInFileName() const;

    /*!
     * Retrieves the filename the neighborhood dot string should be written too.
     *
     * @return the filename.
     */
    std::string getEMdpNeighborhoodDotOutFileName() const;

    /*!
     * Retrieves the state for which the neighborhood should be explored.
     *
     * @return the state.
     */
    uint_fast64_t getEMdpNeighborhoodState() const;

    /*!
     * Retrieves the depth to which neighboors should be explored.
     *
     * @return the depth.
     */
    uint_fast64_t getEMdpNeighborhoodDepth() const;

    /*!
     * Retrieves the filename the dot string should be written too.
     *
     * @return the filename.
     */
    std::string getBMdpDotOutFileName() const;

    /*!
     * Retrieves how often a path is sampled per simulation step.
     *
     * @return The number of sampled paths per simulation step.
     */
    uint_fast64_t getNumberOfSamplingsPerSimulationStep() const;

    /*!
     * Retrieves the selected next-state heuristic.
     *
     * @return The selected next-state heuristic.
     */
    storm::modelchecker::blackbox::heuristicSim::HeuristicSimType getSimulationHeuristicType() const;

    /*!
     * Retrieves a seed for the simulation heuristic.
     *
     * @return seed for random generator
     */
    std::seed_seq getSimHeuristicSeed() const;

    /*!
     * Retrieves the selected boundaray function type for infer
     *
     * @return boundaray function type
     */
    BoundFuncType getBoundFuncType() const;

    /*!
     * Retrieves the selected delta distribution type for infer
     *
     * @return delta distribution type
     */
    DeltaDistType getDeltaDistType() const;

    /*!
     * Retrieves the amount of uncertainty that ishould be used for infer
     *
     * @return delta value for infer
     */
    double getDelta() const;

    /*!
     * Retrieves lower bound for all transition probabilities in the blackbox mdp.
     *
     * @return lower bound for all transition probabilities.
     */
    double getPMin() const;

    /*!
     * Retrieves the desired precision to achieve with value approximation.
     *
     * @return The precision to achieve with value approximation.
     */
    double getPrecision() const;

    bool getIsGreybox() const;

    /*!
     * Retrieves maximum number of algorithm iterations.
     *
     * @return max iterations
     */
    uint_fast64_t getMaxIterations() const;

    virtual bool check() const override;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.

    // visualize emdp constants
    static const std::string printEMdpOptionName;
    static const std::string writeEMdpToFile;
    static const std::string convertToDotEMdpOptionName;
    static const std::string convertDotNeighborhoodEMdpOptionName;
    static const std::string dotIncludeActionsOptionName;
    static const std::string dotIncludeSamplesOptionName;
    static const std::string dotIncludeLabelsOptionName;
    static const std::string dotIncludeColorOptionName;

    // visualize bdmp constants
    static const std::string printBMdpOptionName;
    static const std::string convertToDotBMdpOptionName;

    // simulation step constants
    static const std::string numberOfSamplingsPerSimulationStepOptionName;
    static const std::string simulationHeuristicOptionName;
    static const std::string seedSimHeuristicOptionName;  // TODO replace with one general seed

    // infer constants
    static const std::string deltaDistributionOptionName;
    static const std::string deltaOptionName;
    static const std::string boundFuncOptionName;
    static const std::string greyboxOptionName;

    // general constants
    static const std::string pMinOptionName;
    static const std::string precisionOptionName;
    static const std::string precisionOptionShortName;
    static const std::string maxNumIterationsOptionName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm

#endif /* STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_ */