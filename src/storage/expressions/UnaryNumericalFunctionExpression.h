#ifndef STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_

#include "src/storage/expressions/UnaryExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class UnaryNumericalFunctionExpression : public UnaryExpression {
        public:
            /*!
             * An enum type specifying the different functions applicable.
             */
            enum class OperatorType {Minus, Floor, Ceil};
            
            /*!
             * Creates a unary numerical function expression with the given return type, operand and operator.
             *
             * @param returnType The return type of the expression.
             * @param operand The operand of the expression.
             * @param operatorType The operator of the expression.
             */
            UnaryNumericalFunctionExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand, OperatorType operatorType);
            
            // Instantiate constructors and assignments with their default implementations.
            UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression const& other) = default;
            UnaryNumericalFunctionExpression& operator=(UnaryNumericalFunctionExpression const& other) = default;
#ifndef WINDOWS
            UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression&&) = default;
            UnaryNumericalFunctionExpression& operator=(UnaryNumericalFunctionExpression&&) = default;
#endif
            virtual ~UnaryNumericalFunctionExpression() = default;
            
            // Override base class methods.
            virtual storm::expressions::OperatorType getOperator() const override;
            virtual int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const override;
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
     
            /*!
             * Retrieves the operator associated with this expression.
             *
             * @return The operator associated with this expression.
             */
            OperatorType getOperatorType() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;

        private:
            // The operator of this expression.
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_ */