#include "storm/storage/expressions/ToDiceStringVisitor.h"

namespace storm {
    namespace expressions {
        ToDiceStringVisitor::ToDiceStringVisitor(uint64 nrBits) : nrBits(nrBits) {

        }

        std::string ToDiceStringVisitor::toString(Expression const& expression) {
            return toString(expression.getBaseExpressionPointer().get());
        }

        std::string ToDiceStringVisitor::toString(BaseExpression const* expression) {
            stream.str("");
            stream.clear();
            expression->accept(*this, boost::none);
            return stream.str();
        }

        boost::any ToDiceStringVisitor::visit(IfThenElseExpression const& expression, boost::any const& data) {
            stream << "if ";
            expression.getCondition()->accept(*this, data);
            stream << " then ";
            expression.getThenExpression()->accept(*this, data);
            stream << " else ";
            expression.getElseExpression()->accept(*this, data);
            stream << "";
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case BinaryBooleanFunctionExpression::OperatorType::And:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " && ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Or:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " || ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Xor:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " xor ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Implies:
                    stream << "(!(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ") || ";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryBooleanFunctionExpression::OperatorType::Iff:
                    expression.getFirstOperand()->accept(*this, data);
                    stream << " <=> ";
                    expression.getSecondOperand()->accept(*this, data);
                    break;
            }
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "+";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "-";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "*";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "/";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "^";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Modulo:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "%";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Max:
                    stream << "max(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ",";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Min:
                    stream << "min(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ",";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
            }
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(BinaryRelationExpression const& expression, boost::any const& data) {
            switch (expression.getRelationType()) {
                case BinaryRelationExpression::RelationType::Equal:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "==";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::NotEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "!=";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Less:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "<";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::LessOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << "<=";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::Greater:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ">";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case BinaryRelationExpression::RelationType::GreaterOrEqual:
                    stream << "(";
                    expression.getFirstOperand()->accept(*this, data);
                    stream << ">=";
                    expression.getSecondOperand()->accept(*this, data);
                    stream << ")";
                    break;
            }
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(VariableExpression const& expression, boost::any const&) {
            stream << expression.getVariable().getName();
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case UnaryBooleanFunctionExpression::OperatorType::Not:
                    stream << "!(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
            }
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            switch (expression.getOperatorType()) {
                case UnaryNumericalFunctionExpression::OperatorType::Minus:
                    stream << "-(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Floor:
                    stream << "floor(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
                case UnaryNumericalFunctionExpression::OperatorType::Ceil:
                    stream << "ceil(";
                    expression.getOperand()->accept(*this, data);
                    stream << ")";
                    break;
            }
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(BooleanLiteralExpression const& expression, boost::any const&) {
            stream << (expression.getValue() ? " true " : " false ");
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(IntegerLiteralExpression const& expression, boost::any const&) {
            stream << "int(" <<  nrBits << "," <<  expression.getValue() << ")";
            return boost::any();
        }

        boost::any ToDiceStringVisitor::visit(RationalLiteralExpression const& expression, boost::any const&) {
            stream << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10) << "(" << expression.getValueAsDouble() << ")";
            return boost::any();
        }
    }
}
