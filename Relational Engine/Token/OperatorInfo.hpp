#pragma once
#include <string>
#include <vector>
#include "TokenEnums.hpp"

/**
 * @file OperatorInfo.hpp
 * @brief Definition of the OperatorInfo class for SQL operator metadata.
 * @details
 * Stores metadata for SQL operators (built-in or user-defined/overloaded), such as symbol, precedence,
 * associativity, arity, return type, argument types, and user-defined status.
 */

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
    OperatorCategory category;              ///< Operator category (arithmetic, logical, etc.)
    std::string symbol;                     ///< Operator symbol (e.g. "+", "AND", "->")
    int precedence;                         ///< Operator precedence (higher means binds tighter)
    bool isLeftAssociative;                 ///< True if operator is left associative
    int arity;                              ///< Operator arity (1 for unary, 2 for binary, etc.)
    std::string returnType;                 ///< Return type of the operator
    std::vector<std::string> argumentTypes; ///< List of argument types
    bool isUserDefined;                     ///< True if the operator is user-defined or overloaded

    /**
     * @brief Constructs an OperatorInfo object.
     * @param category_ Operator category.
     * @param symbol_ Operator symbol.
     * @param precedence_ Operator precedence.
     * @param isLeftAssociative_ Is left associative.
     * @param arity_ Operator arity.
     * @param returnType_ Return type.
     * @param argumentTypes_ Argument types.
     * @param isUserDefined_ Is user-defined.
     */
    OperatorInfo(
        OperatorCategory category_ = OperatorCategory::UNKNOWN,
        std::string symbol_ = "",
        int precedence_ = 0,
        bool isLeftAssociative_ = true,
        int arity_ = 2,
        std::string returnType_ = "",
        std::vector<std::string> argumentTypes_ = {},
        bool isUserDefined_ = false
    )
        : category(category_), symbol(std::move(symbol_)), precedence(precedence_),
        isLeftAssociative(isLeftAssociative_), arity(arity_),
        returnType(std::move(returnType_)), argumentTypes(std::move(argumentTypes_)),
        isUserDefined(isUserDefined_) {
    }

    OperatorInfo(const OperatorInfo&) = default;
    OperatorInfo(OperatorInfo&&) = default;
    OperatorInfo& operator=(const OperatorInfo&) = default;
    OperatorInfo& operator=(OperatorInfo&&) = default;
    ~OperatorInfo() = default;
};