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

// TODO clarfify if lowercase or camelcase should be used for options
const std::string BlackboxSettings::moduleName = "blackbox";

// visualize emdp constants 
const std::string BlackboxSettings::printEMdpOptionName = "printeMDP";
const std::string BlackboxSettings::writeEMdpToFile = "eMDPtoFile";
const std::string BlackboxSettings::convertToDotEMdpOptionName = "eMDPtoDot";
const std::string BlackboxSettings::convertDotNeighborhoodEMdpOptionName = "eMDPNeighbToDot";
const std::string BlackboxSettings::dotIncludeActionsOptionName = "eMDPDotIncAct";
const std::string BlackboxSettings::dotIncludeSamplesOptionName = "eMDPDotIncSmpl"; 
const std::string BlackboxSettings::dotIncludeLabelsOptionName = "eMDPDotIncLab";
const std::string BlackboxSettings::dotIncludeColorOptionName = "eMDPDotIncCol";

// visualize bmdp constants 
const std::string BlackboxSettings::printBMdpOptionName = "printbMDP";
const std::string BlackboxSettings::convertToDotBMdpOptionName = "bMDPtoDot"; 

// simulation constants
const std::string BlackboxSettings::numberOfSamplingsPerSimulationStepOptionName = "stepssim";
const std::string BlackboxSettings::simulationHeuristicOptionName = "simheuristic";
const std::string BlackboxSettings::seedSimHeuristicOptionName = "seedsimheuristic";
// infer constants
const std::string BlackboxSettings::deltaDistributionOptionName = "deltadist";
const std::string BlackboxSettings::boundFuncOptionName = "boundfunc";
// general constants
const std::string BlackboxSettings::pMinOptionName = "pmin";
const std::string BlackboxSettings::precisionOptionName = "precision";
const std::string BlackboxSettings::precisionOptionShortName = "eps";
const std::string BlackboxSettings::maxNumIterationsOptionName = "maxiterations";


BlackboxSettings::BlackboxSettings() : ModuleSettings(moduleName) {
    // visualize emdp options 
    this->addOption(storm::settings::OptionBuilder(moduleName, printEMdpOptionName, true,
                                                   "Prints the eMDP to the console.")
                        .setIsAdvanced()
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, writeEMdpToFile, true, "Writes the eMDP to a File.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "filename",
                                         "The name of the file to write the eMDP to. 'eMDP.txt' default.")
                                         .setDefaultValueString("eMDP.txt")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, convertToDotEMdpOptionName, true, "Converts the eMDP to the dot format.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "eMDPInfile",
                                         "The name of the file to read the eMDP from. If not set converts the currently explored eMDP.")
                                         .setDefaultValueString("explored")
                                         .build())
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "outstream",
                                         "The name of the file to write the dot string to. If not set prints the dot string to the console.")
                                         .setDefaultValueString("console")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, convertDotNeighborhoodEMdpOptionName, true, "Converts the eMDP to the dot format.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "eMDPInfile",
                                         "The name of the file to read the eMDP from.")
                                         .build())
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "outstream",
                                         "The name of the file to write the dot string to. If not set prints the dot string to the console.")
                                         .setDefaultValueString("console")
                                         .build())
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("state", "State for which neighborhood should be visualized.")
                                         .setDefaultValueUnsignedInteger(0)
                                         .build())
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("depth", "Depth to which neighborhood should be explored.")
                                         .setDefaultValueUnsignedInteger(3)
                                         .build())
                        .build());    

    this->addOption(storm::settings::OptionBuilder(moduleName, dotIncludeActionsOptionName, true,
                                                   "Includes actions when converting the eMDP to the dot format.")
                        .setIsAdvanced()
                        .build());    

    this->addOption(storm::settings::OptionBuilder(moduleName, dotIncludeSamplesOptionName, true,
                                                   "Includes Samples when converting the eMDP to the dot format.")
                        .setIsAdvanced()
                        .build());   

    this->addOption(storm::settings::OptionBuilder(moduleName, dotIncludeLabelsOptionName, true,
                                                   "Includes Labels when converting the eMDP to the dot format.")
                        .setIsAdvanced()
                        .build());   

    this->addOption(storm::settings::OptionBuilder(moduleName, dotIncludeColorOptionName, true,
                                                   "Includes Color when converting the eMDP to the dot format.")
                        .setIsAdvanced()
                        .build());         

    // visualize bmdp options 

    this->addOption(storm::settings::OptionBuilder(moduleName, printBMdpOptionName, true,
                                                   "Prints the bMDP to the console.")
                        .setIsAdvanced()
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, convertToDotBMdpOptionName, true, "Converts the bMDP to the dot format.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "outstream",
                                         "The name of the file to write the dot string to. If not set prints the dot string to the console.")
                                         .setDefaultValueString("console")
                                         .build())
                        .build());



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
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "Lower bound for all transition probabilities.")
                                         .setDefaultValueDouble(1e-06)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
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

    this->addOption(storm::settings::OptionBuilder(moduleName, maxNumIterationsOptionName, true, "Maximal number of iterations the 3 step algorithm is performe.")
                        .setShortName(precisionOptionShortName)
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxIter", "Maximum number of algorithm iterations.")
                                         .setDefaultValueUnsignedInteger(5)
                                         .build())
                        .build());                        
}

std::string BlackboxSettings::getEMdpOutFileName() const {
    return this->getOption(printEMdpOptionName).getArgumentByName("filename").getValueAsString();
}

std::string BlackboxSettings::getEMdpDotInFileName() const {
    return this->getOption(convertToDotEMdpOptionName).getArgumentByName("eMDPInfile").getValueAsString();
}

std::string BlackboxSettings::getEMdpDotOutFileName() const {
    return this->getOption(convertToDotEMdpOptionName).getArgumentByName("outstream").getValueAsString();
}

std::string BlackboxSettings::getEMdpNeighbDotInFileName() const {
    return this->getOption(convertDotNeighborhoodEMdpOptionName).getArgumentByName("eMDPInfile").getValueAsString();
}

std::string BlackboxSettings::getEMdpNeighbDotOutFileName() const {
    return this->getOption(convertDotNeighborhoodEMdpOptionName).getArgumentByName("outstream").getValueAsString();
}

std::string BlackboxSettings::getBMdpDotOutFileName() const {
    return this->getOption(convertToDotBMdpOptionName).getArgumentByName("outstream").getValueAsString();
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
    std::string boundFuncStr = this->getOption(deltaDistributionOptionName).getArgumentByName("deltaDist").getValueAsString();
    if (boundFuncStr == "hoeffding") {
        return BoundFuncType::HOEFFDING;
    } else if (boundFuncStr == "oshoeffding") {
        return BoundFuncType::ONESIDEDHOEFFDING;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown boundary function type '" << boundFuncStr << "'.");
};

DeltaDistType BlackboxSettings::getDeltaDistType() const {
    std::string deltaDistStr = this->getOption(boundFuncOptionName).getArgumentByName("boundFunc").getValueAsString();
    if (deltaDistStr == "uniform") {
        return DeltaDistType::UNIFORM;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown delta distribution type '" << deltaDistStr << "'.");
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
                      this->getOption(maxNumIterationsOptionName).getHasOptionBeenSet() ||
                      
                      this->getOption(printEMdpOptionName).getHasOptionBeenSet() ||
                      this->getOption(writeEMdpToFile).getHasOptionBeenSet() ||
                      this->getOption(convertToDotEMdpOptionName).getHasOptionBeenSet() ||
                      this->getOption(convertDotNeighborhoodEMdpOptionName).getHasOptionBeenSet() ||
                      this->getOption(dotIncludeActionsOptionName).getHasOptionBeenSet() ||
                      this->getOption(dotIncludeSamplesOptionName).getHasOptionBeenSet() ||
                      this->getOption(dotIncludeLabelsOptionName).getHasOptionBeenSet() ||
                      this->getOption(dotIncludeColorOptionName).getHasOptionBeenSet() ||
                      this->getOption(printBMdpOptionName).getHasOptionBeenSet() ||
                      this->getOption(convertToDotBMdpOptionName).getHasOptionBeenSet();
    STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::utility::Engine::Blackbox || !optionsSet,
                        "blackbox engine is not selected, so setting options for it has no effect.");
    return true;
}
}  // namespace modules
}  // namespace settings
}  // namespace storm