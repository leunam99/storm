#ifndef STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_
#define STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_

#include <string>
#include <map>
#include <boost/optional.hpp>

#include "src/storage/prism/Composition.h"

namespace storm {
    namespace prism {
        class RenamingComposition : public Composition {
        public:
            RenamingComposition(std::shared_ptr<Composition> const& sub, std::map<std::string, std::string> const& actionRenaming);
            
            virtual boost::any accept(CompositionVisitor& visitor) const override;
            
            Composition const& getSubcomposition() const;
            
            std::map<std::string, std::string> const& getActionRenaming() const;
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
            
        private:
            std::shared_ptr<Composition> sub;
            
            // The renaming of action indices to apply. If the target name is none, the action is hidden.
            std::map<std::string, std::string> actionRenaming;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_RENAMINGCOMPOSITION_H_ */
