#ifndef STORM_STORAGE_PRISM_MODULE_H_
#define STORM_STORAGE_PRISM_MODULE_H_

#include <set>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <boost/container/flat_set.hpp>

#include "src/storage/prism/BooleanVariable.h"
#include "src/storage/prism/IntegerVariable.h"
#include "src/storage/prism/Command.h"
#include "src/storage/expressions/VariableExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class Module : public LocatedInformation {
        public:
            /*!
             * Creates a module with the given name, variables and commands.
             *
             * @param moduleName The name of the module.
             * @param booleanVariables The boolean variables defined by the module.
             * @param integerVariables The integer variables defined by the module.
             * @param commands The commands of the module.
             * @param filename The filename in which the module is defined.
             * @param lineNumber The line number in which the module is defined.
             */
            Module(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& filename = "", uint_fast64_t lineNumber = 0);

            /*!
             * Creates a module with the given name, variables and commands that is marked as being renamed from the
             * given module with the given renaming.
             *
             * @param moduleName The name of the module.
             * @param booleanVariables The boolean variables defined by the module.
             * @param integerVariables The integer variables defined by the module.
             * @param commands The commands of the module.
             * @param renamedFromModule The name of the module from which this module was renamed.
             * @param renaming The renaming of identifiers used to create this module.
             * @param filename The filename in which the module is defined.
             * @param lineNumber The line number in which the module is defined.
             */
            Module(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& renamedFromModule, std::map<std::string, std::string> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);

            // Create default implementations of constructors/assignment.
            Module() = default;
            Module(Module const& other) = default;
            Module& operator=(Module const& other)= default;
#ifndef WINDOWS
            Module(Module&& other) = default;
            Module& operator=(Module&& other) = default;
#endif
            
            /*!
             * Retrieves the number of boolean variables in the module.
             *
             * @return the number of boolean variables in the module.
             */
            std::size_t getNumberOfBooleanVariables() const;
            
            /*!
             * Retrieves the number of integer variables in the module.
             *
             * @return The number of integer variables in the module.
             */
            std::size_t getNumberOfIntegerVariables() const;
            
            /*!
             * Retrieves a reference to the boolean variable with the given name.
             *
             * @param variableName The name of the boolean variable to retrieve.
             * @return A reference to the boolean variable with the given name.
             */
            storm::prism::BooleanVariable const& getBooleanVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the boolean variables of the module.
             *
             * @return The boolean variables of the module.
             */
            std::vector<storm::prism::BooleanVariable> const& getBooleanVariables() const;
            
            /*!
             * Retrieves a reference to the integer variable with the given name.
             *
             * @param variableName The name of the integer variable to retrieve.
             * @return A reference to the integer variable with the given name.
             */
            storm::prism::IntegerVariable const& getIntegerVariable(std::string const& variableName) const;

            /*!
             * Retrieves the integer variables of the module.
             *
             * @return The integer variables of the module.
             */
            std::vector<storm::prism::IntegerVariable> const& getIntegerVariables() const;

            /*!
             * Retrieves the number of commands of this module.
             *
             * @return The number of commands of this module.
             */
            std::size_t getNumberOfCommands() const;
            
            /*!
             * Retrieves the total number of updates of this module.
             *
             * @return The total number of updates of this module.
             */
            std::size_t getNumberOfUpdates() const;
            
            /*!
             * Retrieves a reference to the command with the given index.
             *
             * @param index The index of the command to retrieve.
             * @return A reference to the command with the given index.
             */
            storm::prism::Command const& getCommand(uint_fast64_t index) const;
            
            /*!
             * Retrieves the commands of the module.
             *
             * @return The commands of the module.
             */
            std::vector<storm::prism::Command> const& getCommands() const;
            
            /*!
             * Retrieves the name of the module.
             *
             * @return The name of the module.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the set of actions present in this module.
             *
             * @return the set of actions present in this module.
             */
            std::set<std::string> const& getActions() const;
            
            /*!
             * Retrieves whether or not this module contains a command labeled with the given action.
             *
             * @param action The action name to look for in this module.
             * @return True iff the module has at least one command labeled with the given action.
             */
            bool hasAction(std::string const& action) const;
            
            /*!
             * Retrieves whether this module was created from another module via renaming.
             *
             * @return True iff the module was created via renaming.
             */
            bool isRenamedFromModule() const;
            
            /*!
             * If the module was created via renaming, this method retrieves the name of the module that was used as the
             * in the base in the renaming process.
             *
             * @return The name of the module from which this module was created via renaming.
             */
            std::string const& getBaseModule() const;
            
            /*!
             * If the module was created via renaming, this method returns the applied renaming of identifiers used for
             * the renaming process.
             *
             * @return A mapping of identifiers to new identifiers that was used in the renaming process.
             */
            std::map<std::string, std::string> const& getRenaming() const;
            
            /*!
             * Retrieves the indices of all commands within this module that are labelled by the given action.
             *
             * @param action The action with which the commands have to be labelled.
             * @return A set of indices of commands that are labelled with the given action.
             */
            std::set<uint_fast64_t> const& getCommandIndicesByAction(std::string const& action) const;
            
            /*!
             * Creates a new module that drops all commands whose indices are not in the given set.
             *
             * @param indexSet The set of indices for which to keep the commands.
             * @return The module resulting from erasing all commands whose indices are not in the given set.
             */
            Module restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) const;
            
            /*!
             * Substitutes all identifiers in the module according to the given map.
             *
             * @param substitution The substitution to perform.
             * @return The resulting module.
             */
            Module substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const;
            
            friend std::ostream& operator<<(std::ostream& stream, Module const& module);

        private:
            /*!
             * Computes the locally maintained mappings for fast data retrieval.
             */
            void createMappings();
            
            // The name of the module.
            std::string moduleName;
            
            // A list of boolean variables.
            std::vector<storm::prism::BooleanVariable> booleanVariables;
            
            // A mapping from boolean variables to the corresponding indices in the vector.
            std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
            
            // A list of integer variables.
            std::vector<storm::prism::IntegerVariable> integerVariables;

            // A mapping from integer variables to the corresponding indices in the vector.
            std::map<std::string, uint_fast64_t> integerVariableToIndexMap;

            // The commands associated with the module.
            std::vector<storm::prism::Command> commands;
            
            // The set of actions present in this module.
            std::set<std::string> actions;
            
            // A map of actions to the set of commands labeled with this action.
            std::map<std::string, std::set<uint_fast64_t>> actionsToCommandIndexMap;
            
            // This string indicates whether and from what module this module was created via renaming.
            std::string renamedFromModule;
            
            // If the module was created by renaming, this mapping contains the provided renaming of identifiers.
            std::map<std::string, std::string> renaming;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_MODULE_H_ */