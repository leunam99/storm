#include "storm/settings/modules/BlackboxSettings.h"
#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/Engine.h"
#include "storm/utility/macros.h"

namespace storm {
namespace settings {
namespace modules {

// TODO add flag
/*
print eMDP, bMDP
print to Dot
    emdp
    bmdp
io eMDP
    neighbours
save_memory -> bool, use uint32 or uint64
*/

const std::string BlackboxSettings::moduleName = "blackbox";
// simulation constants
const std::string BlackboxSettings::numberOfSamplingsPerSimulationStepOptionName = "stepssim";
const std::string BlackboxSettings::simulationHeuristicOptionName = "simheuristic";
const std::string BlackboxSettings::seedSimHeuristicOptionName = "seedsimheuristic";
// plot simulation
// infer constants
const std::string BlackboxSettings::deltaDistributionOptionName = "deltadist";
const std::string BlackboxSettings::deltaOptionName = "delta";
const std::string BlackboxSettings::boundFuncOptionName = "boundfunc";
// general constants
const std::string BlackboxSettings::pMinOptionName = "pmin";
const std::string BlackboxSettings::precisionOptionName = "precision";
const std::string BlackboxSettings::precisionOptionShortName = "eps";
const std::string BlackboxSettings::maxNumIterationsOptionName = "maxiterations";


BlackboxSettings::BlackboxSettings() : ModuleSettings(moduleName) {
    // simulation options
    this->addOption(storm::settings::OptionBuilder(moduleName, numberOfSamplingsPerSimulationStepOptionName, true,
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

    this->addOption(storm::settings::OptionBuilder(moduleName, seedSimHeuristicOptionName, true, "Set seed used to initialize simulation heuristic. 'default' tells the program to use the current datetime.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "seed",
                                         "The seed used to initalize the simulation heuristic.")
                                         .setDefaultValueString("default")
                                         .build())
                        .build());
    
    // infer options
    std::vector<std::string> deltaDistributions = {"uniform"};
    this->addOption(storm::settings::OptionBuilder(moduleName, deltaDistributionOptionName, true, "Set delta distribution used to distribute uncertainty on bounds during infer stage.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "deltaDist",
                                         "delta distribution used during infer stage.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(deltaDistributions))
                                         .setDefaultValueString("uniform")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, deltaOptionName, true, "Set uncertainty of bounds created for the BMdp during infer stage.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument(
                                         "delta",
                                         "uncertainty of bounds of created BMdp.")
                                         .setDefaultValueDouble(0.3)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
                                         .build())
                        .build());

    std::vector<std::string> boundFunctions = {"hoeffding", "oshoeffding"};
    this->addOption(storm::settings::OptionBuilder(moduleName, boundFuncOptionName, true, "Set boundary function used to create bounds of bounded mdp.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "boundFunc",
                                         "Boundary function used during infer stage.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(boundFunctions))
                                         .setDefaultValueString("hoeffding")
                                         .build())
                        .build());

    // general options
    this->addOption(storm::settings::OptionBuilder(moduleName, pMinOptionName, false, "Lower bound for all transition probabilities in the blackbox mdp.")
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("pMin", "Lower bound for all transition probabilities.")
                                         .setDefaultValueDouble(1e-06)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision to achieve. (To be implemented)")
                        .setShortName(precisionOptionShortName)
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("eps", "The value to use to determine convergence.")
                                         .setDefaultValueDouble(1e-06)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, maxNumIterationsOptionName, true, "Maximal number of iterations the 3 step algorithm is performe.")
                        .setShortName(precisionOptionShortName)
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxIter", "Maximum number of algorithm iterations.")
                                         .setDefaultValueUnsignedInteger(5)
                                         .build())
                        .build());                        
}

uint_fast64_t BlackboxSettings::getNumberOfSamplingsPerSimulationStep() const {
    return this->getOption(numberOfSamplingsPerSimulationStepOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
}

uint_fast64_t BlackboxSettings::getMaxIterations() const {
    return this->getOption(maxNumIterationsOptionName).getArgumentByName("maxIter").getValueAsUnsignedInteger();
}

storm::modelchecker::blackbox::heuristicSim::HeuristicSimType BlackboxSettings::getSimulationHeuristicType() const {
    std::string simulationHeuristicType = this->getOption(simulationHeuristicOptionName).getArgumentByName("name").getValueAsString();
    if (simulationHeuristicType == "naive") {
        return storm::modelchecker::blackbox::heuristicSim::HeuristicSimType::NAIVE;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown simulation heuristic type '" << simulationHeuristicType << "'.");
}

std::seed_seq BlackboxSettings::getSimHeuristicSeed() const {
    std::string seedStr = this->getOption(seedSimHeuristicOptionName).getArgumentByName("seed").getValueAsString();
    if (seedStr == "default") {
        return std::seed_seq({std::chrono::system_clock::now().time_since_epoch().count()});
    }
    return std::seed_seq(seedStr.begin(), seedStr.end());
}

BoundFuncType BlackboxSettings::getBoundFuncType() const {
    std::string boundFuncStr = this->getOption(boundFuncOptionName).getArgumentByName("boundFunc").getValueAsString();
    if (boundFuncStr == "hoeffding") {
        return BoundFuncType::HOEFFDING;
    } else if (boundFuncStr == "oshoeffding") {
        return BoundFuncType::ONESIDEDHOEFFDING;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown boundary function type '" << boundFuncStr << "'.");
};

DeltaDistType BlackboxSettings::getDeltaDistType() const {
    std::string deltaDistStr = this->getOption(deltaDistributionOptionName).getArgumentByName("deltaDist").getValueAsString();
    if (deltaDistStr == "uniform") {
        return DeltaDistType::UNIFORM;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown delta distribution type '" << deltaDistStr << "'.");
};

double BlackboxSettings::getDelta() const {
    return this->getOption(deltaOptionName).getArgumentByName("delta").getValueAsDouble();
};

double BlackboxSettings::getPMin() const {
    return this->getOption(pMinOptionName).getArgumentByName("value").getValueAsDouble();
}


double BlackboxSettings::getPrecision() const {
    return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
}

bool BlackboxSettings::check() const {
    bool optionsSet = this->getOption(numberOfSamplingsPerSimulationStepOptionName).getHasOptionBeenSet() ||
                      this->getOption(simulationHeuristicOptionName).getHasOptionBeenSet() ||
                      this->getOption(seedSimHeuristicOptionName).getHasOptionBeenSet() ||
                      this->getOption(deltaDistributionOptionName).getHasOptionBeenSet() ||
                      this->getOption(boundFuncOptionName).getHasOptionBeenSet() ||
                      this->getOption(precisionOptionName).getHasOptionBeenSet() ||
                      this->getOption(maxNumIterationsOptionName).getHasOptionBeenSet();
    STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::utility::Engine::Blackbox || !optionsSet,
                        "blackbox engine is not selected, so setting options for it has no effect.");
    return true;
}
}  // namespace modules
}  // namespace settings
}  // namespace storm