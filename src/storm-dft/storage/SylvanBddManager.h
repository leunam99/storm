#pragma once

#include <map>

#include "storm/storage/dd/sylvan/InternalSylvanDdManager.h"
#include "storm/utility/macros.h"
#include "storm/utility/sylvan.h"

namespace storm {
namespace storage {

/**
 * Simple Manager for Sylvan
 *
 * \note
 * Not thread safe.
 *
 * Initialises Sylvan
 * which can only be initialize once per programm invocation
 */
class SylvanBddManager {
   public:
    /**
     * Initilizes Sylvan
     *
     * \note
     * Internally Sylvan is initialiesed by a InternalSylvanDdManager.
     * This ensures compatibility.
     */
    SylvanBddManager() = default;

    // We can only initialize Sylvan once therefore no copy semantics
    SylvanBddManager(SylvanBddManager const &) = delete;

    SylvanBddManager(SylvanBddManager &&) = default;
    SylvanBddManager &operator=(SylvanBddManager &&) = default;

    /**
     * Destroys Sylvan
     */
    ~SylvanBddManager() = default;

    /**
     * Creates a variable with a unique name
     *
     * \return Index of the variable
     */
    uint32_t createVariable(std::string const name) {
        nameToIndex[name] = nextFreeVariableIndex;
        indexToName[nextFreeVariableIndex] = name;
        sylvan::Bdd::bddVar(nextFreeVariableIndex);
        return nextFreeVariableIndex++;
    }

    /**
     * \return
     * The bdd representation of the variable.
     * Therefore ITE(x, 1, 0).
     */
    sylvan::Bdd getPositiveLiteral(std::string const name) const {
        return sylvan::Bdd::bddVar(nameToIndex.at(name));
    }

    /**
     * \note equivalent with !getPositiveLiteral(name)
     *
     * \return
     * The negative bdd representation of the variable.
     * Therefore ITE(x, 0, 1).
     */
    sylvan::Bdd getNegativeLiteral(std::string const name) const {
        return sylvan::Bdd::bddVar(nameToIndex.at(name));
    }

    /**
     * \return
     * The bdd representation of the variable.
     * Therefore ITE(x, 1, 0).
     */
    sylvan::Bdd getPositiveLiteral(uint32_t const index) const {
        return sylvan::Bdd::bddVar(index);
    }

    /**
     * \note equivalent with !getPositiveLiteral(index)
     *
     * \return
     * The negative bdd representation of the variable.
     * Therefore ITE(x, 0, 1).
     */
    sylvan::Bdd getNegativeLiteral(uint32_t const index) const {
        return sylvan::Bdd::bddVar(index);
    }

    /**
     * \return
     * The bdd represenatation of the constant function 1.
     */
    sylvan::Bdd getOne() { return sylvan::Bdd::bddOne(); }

    /**
     * \return
     * The bdd represenatation of the constant function 1.
     */
    sylvan::Bdd getZero() { return sylvan::Bdd::bddZero(); }

    /**
     * \return
     * The index of the variable with the given name.
     */
    uint32_t getIndex(std::string const name) const {
        return nameToIndex.at(name);
    }

    /**
     * \return
     * The name of the variable with the given index.
     */
    std::string getName(uint32_t const index) const {
        return indexToName.at(index);
    }

    /**
     * Exports the given Bdd to a file
     * in the dot format.
     *
     * \param bdd
     * The Bdd.
     * \param filename
     * The name of the file the dot graph is written to
     */
    static void exportBddToDot(sylvan::Bdd const &bdd,
                               std::string const &filename) {
        FILE *filePointer = fopen(filename.c_str(), "w+");

        // fopen returns a nullptr on failure
        if (filePointer == nullptr) {
            STORM_LOG_ERROR("Failure to open file: " << filename);
        } else {
            bdd.PrintDot(filePointer);
            fclose(filePointer);
        }
    }

   private:
    storm::dd::InternalDdManager<storm::dd::DdType::Sylvan> internalManager{};
    uint32_t nextFreeVariableIndex{0};

    std::map<std::string, uint32_t> nameToIndex{};
    std::map<uint32_t, std::string> indexToName{};
};

}  // namespace storage
}  // namespace storm
