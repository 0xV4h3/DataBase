/**
 * @file OperatorInfo.hpp
 * @brief Definition of the OperatorInfo class for SQL operator metadata.
 * @details
 * Stores metadata for SQL operators (built-in or user-defined/overloaded), such as symbol, precedence,
 * associativity, arity, return type, argument types, and user-defined status.
 */

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "TokenEnums.hpp"

 /**
  * @class OperatorInfo
  * @brief Information about an SQL operator (built-in or user-defined/overloaded).
  * @details
  * Contains metadata including operator symbol, precedence, associativity, arity,
  * return type, argument types, and whether the operator is user-defined.
  *
  * @see OperatorCategory
  */
class OperatorInfo {
public:
    // === Constructors ===

    /**
     * @brief Constructs an OperatorInfo object.
     * @param category_ Operator category
     * @param symbol_ Operator symbol
     * @param precedence_ Operator precedence
     * @param leftAssociative_ Is left associative
     * @param unary_ Is unary operator
     * @param returnType_ Return type
     * @param argumentTypes_ Argument types
     * @param isUserDefined_ Is user-defined
     * @throws std::invalid_argument if symbol is empty or precedence is negative
     */
    OperatorInfo(
        OperatorCategory category_ = OperatorCategory::UNKNOWN,
        std::string symbol_ = "",
        int precedence_ = 0,
        bool leftAssociative_ = true,
        bool unary_ = false,
        std::string returnType_ = "",
        std::vector<std::string> argumentTypes_ = {},
        bool isUserDefined_ = false
    )
        : category(category_)
        , symbol(std::move(symbol_))
        , precedence(precedence_)
        , leftAssociative(leftAssociative_)
        , unary(unary_)
        , returnType(std::move(returnType_))
        , argumentTypes(std::move(argumentTypes_))
        , isUserDefined_(isUserDefined_)  
    {
        validate();
    }

    // Rule of five for proper memory management
    OperatorInfo(const OperatorInfo&) = default;
    OperatorInfo(OperatorInfo&&) noexcept = default;
    OperatorInfo& operator=(const OperatorInfo&) = default;
    OperatorInfo& operator=(OperatorInfo&&) noexcept = default;
    virtual ~OperatorInfo() = default;

    // === Accessors ===

    /**
     * @brief Gets the operator category.
     * @return OperatorCategory enum value
     */
    OperatorCategory getCategory() const { return category; }

    /**
     * @brief Gets the operator symbol.
     * @return Operator symbol string
     */
    const std::string& getSymbol() const { return symbol; }

    /**
     * @brief Gets the operator precedence.
     * @return Precedence value
     */
    int getPrecedence() const { return precedence; }

    /**
     * @brief Gets the return type.
     * @return Return type string
     */
    const std::string& getReturnType() const { return returnType; }

    /**
     * @brief Gets the argument types.
     * @return Vector of argument type strings
     */
    const std::vector<std::string>& getArgumentTypes() const { return argumentTypes; }

    // === Property Checks ===

    /**
     * @brief Checks if operator is left associative.
     * @return true if operator is left associative
     */
    bool isLeftAssociative() const { return leftAssociative; }

    /**
     * @brief Checks if operator is unary.
     * @return true if operator is unary
     */
    bool isUnary() const { return unary; }

    /**
     * @brief Checks if operator is user-defined.
     * @return true if operator is user-defined
     */
    bool isUserDefined() const { return isUserDefined_; }

    /**
     * @brief Gets the operator arity.
     * @return 1 for unary, 2 for binary
     */
    int getArity() const { return unary ? 1 : 2; }

    // === Validation ===

    /**
     * @brief Checks if the operator info is valid.
     * @return true if required fields are valid
     */
    virtual bool isValid() const {
        return !symbol.empty() &&
            category != OperatorCategory::UNKNOWN &&
            precedence >= 0;
    }

    /**
     * @brief Validates the operator info state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const {
        if (symbol.empty()) {
            throw std::invalid_argument("OperatorInfo has empty symbol");
        }
        if (category == OperatorCategory::UNKNOWN) {
            throw std::invalid_argument("OperatorInfo has unknown category");
        }
        if (precedence < 0) {
            throw std::invalid_argument("OperatorInfo has negative precedence");
        }
        if (!returnType.empty() && argumentTypes.empty()) {
            throw std::invalid_argument("OperatorInfo has return type but no argument types");
        }
        if (unary && argumentTypes.size() > 1) {
            throw std::invalid_argument("Unary operator cannot have multiple argument types");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares operator info objects for equality.
     * @param other OperatorInfo to compare with
     * @return true if objects are equal
     */
    virtual bool equals(const OperatorInfo& other) const {
        return category == other.category &&
            symbol == other.symbol &&
            precedence == other.precedence &&
            leftAssociative == other.leftAssociative &&
            unary == other.unary &&
            returnType == other.returnType &&
            argumentTypes == other.argumentTypes &&
            isUserDefined_ == other.isUserDefined_;
    }

    /**
     * @brief Creates a string representation of the operator info.
     * @return Formatted string with operator details
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "OperatorInfo{symbol=\"" << symbol << "\""
            << ", category=" << static_cast<int>(category)
            << ", precedence=" << precedence
            << ", " << (leftAssociative ? "left" : "right") << "-assoc"
            << (unary ? ", unary" : ", binary");

        if (!returnType.empty()) {
            oss << ", returns=\"" << returnType << "\"";
        }
        if (!argumentTypes.empty()) {
            oss << ", args=" << argumentTypes.size();
        }
        if (isUserDefined_) {
            oss << ", user-defined";
        }
        oss << "}";
        return oss.str();  
    }

public:
    OperatorCategory category;              ///< Operator category (arithmetic, logical, etc.)
    std::string symbol;                     ///< Operator symbol (e.g. "+", "AND", "->")
    int precedence;                         ///< Operator precedence (higher means binds tighter)
    bool leftAssociative;                   ///< True if operator is left associative
    bool unary;                             ///< True if operator is unary
    std::string returnType;                 ///< Return type of the operator
    std::vector<std::string> argumentTypes; ///< List of argument types
    bool isUserDefined_;                    ///< True if the operator is user-defined or overloaded
};