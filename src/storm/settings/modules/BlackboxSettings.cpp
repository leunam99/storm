#include "storm/settings/modules/BlackboxSettings.h"
#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/Engine.h"
#include "storm/utility/macros.h"

namespace storm {
namespace settings {
namespace modules {

const std::string BlackboxSettings::moduleName = "blackbox";
const std::string BlackboxSettings::NumberOfSamplingsPerSimulationStepOptionName = "stepssim";
const std::string BlackboxSettings::simulationHeuristicOptionName = "simheuristic";
const std::string BlackboxSettings::precisionOptionName = "precision";
const std::string BlackboxSettings::precisionOptionShortName = "eps";

BlackboxSettings::BlackboxSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, NumberOfSamplingsPerSimulationStepOptionName, true,
                                                   "Sets the number of paths sampled for one simulation step.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of sampled paths per simulation step.")
                                         .setDefaultValueUnsignedInteger(30)
                                         .build())
                        .build());

    std::vector<std::string> simulationHeuristics = {"naive"};
    this->addOption(storm::settings::OptionBuilder(moduleName, simulationHeuristicOptionName, true, "Sets the heuristic used for simulation.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "name",
                                         "The name of the heuristic to use. 'naive' each time chooses a random action.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(simulationHeuristics))
                                         .setDefaultValueString("naive")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision to achieve. (To be implemented)")
                        .setShortName(precisionOptionShortName)
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The value to use to determine convergence.")
                                         .setDefaultValueDouble(1e-06)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
                                         .build())
                        .build());
}

uint_fast64_t BlackboxSettings::getNumberOfSamplingsPerSimulationStep() const {
    return this->getOption(NumberOfSamplingsPerSimulationStepOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
}

storm::modelchecker::blackbox::heuristic_simulate::HeuristicsSim BlackboxSettings::getSimulationHeuristic() const {
    std::string simulationHeuristic = this->getOption(simulationHeuristicOptionName).getArgumentByName("name").getValueAsString();
    if (simulationHeuristic == "naive") {
        return storm::modelchecker::blackbox::heuristic_simulate::HeuristicsSim::NAIVE;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown simulation heuristic '" << simulationHeuristic << "'.");
}

double BlackboxSettings::getPrecision() const {
    return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
}

bool BlackboxSettings::check() const {
    bool optionsSet = this->getOption(NumberOfSamplingsPerSimulationStepOptionName).getHasOptionBeenSet() ||
                      this->getOption(simulationHeuristicOptionName).getHasOptionBeenSet() ||
                      this->getOption(precisionOptionName).getHasOptionBeenSet();
    STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::utility::Engine::Blackbox || !optionsSet,
                        "blackbox engine is not selected, so setting options for it has no effect.");
    return true;
}
}  // namespace modules
}  // namespace settings
}  // namespace storm