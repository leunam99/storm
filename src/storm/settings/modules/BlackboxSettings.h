#ifndef STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_
#define STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"

namespace storm {
namespace settings {
namespace modules {

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
     * Retrieves the precision to use for numerical operations.
     *
     * @return The precision to use for numerical operations.
     */
    double getPrecision() const;

    virtual bool check() const override;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.
    static const std::string NumberOfSamplingsPerSimulationStepOptionName;
    static const std::string simulationHeuristicOptionName;
    static const std::string precisionOptionName;
    static const std::string precisionOptionShortName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm

#endif /* STORM_SETTINGS_MODULES_BLACKBOXSETTINGS_H_ */