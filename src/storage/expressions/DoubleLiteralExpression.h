#ifndef STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class DoubleLiteralExpression : public BaseExpression {
        public:
            /*!
             * Creates an double literal expression with the given value.
             *
             * @param value The value of the double literal.
             */
            DoubleLiteralExpression(double value);
            
            // Instantiate constructors and assignments with their default implementations.
            DoubleLiteralExpression(DoubleLiteralExpression const& other) = default;
            DoubleLiteralExpression& operator=(DoubleLiteralExpression const& other) = default;
#ifndef WINDOWS
            DoubleLiteralExpression(DoubleLiteralExpression&&) = default;
            DoubleLiteralExpression& operator=(DoubleLiteralExpression&&) = default;
#endif
            virtual ~DoubleLiteralExpression() = default;
            
            // Override base class methods.
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual bool isLiteral() const override;
			virtual std::set<std::string> getVariables() const override;
			virtual std::map<std::string, ExpressionReturnType> getVariablesAndTypes() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;

            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            double getValue() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;
            
        private:
            // The value of the double literal.
            double value;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_ */