#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TokenEnums.hpp"

class ASTNode;

/**
 * @file FunctionInfo.hpp
 * @brief Definition of the FunctionInfo class for SQL function metadata.
 * @details
 * Stores metadata about SQL functions (built-in or user-defined), such as their name,
 * category, arguments, return type, and properties like determinism, aggregation, and windowing.
 * Also supports user-defined functions with AST bodies.
 */

 /**
  * @class FunctionInfo
  * @brief Information about a SQL function (built-in or user-defined).
  * @details
  * Contains metadata such as function name, category, argument types, return type,
  * and various function properties. If the function is user-defined, its parsed AST is also stored.
  *
  * @see FunctionCategory
  * @see ASTNode
  */
class FunctionInfo {
public:
    FunctionCategory category;              ///< Function category (aggregate, scalar, etc.)
    std::string name;                       ///< Name of the function.
    std::string returnType;                 ///< Return type of the function.
    std::vector<std::string> argumentTypes; ///< List of argument types.
    bool isAggregate;                       ///< True if function is an aggregate.
    bool isWindow;                          ///< True if function is a window function.
    bool isDeterministic;                   ///< True if function is deterministic.
    bool isUserDefined;                     ///< True if function is user-defined.
    std::string signature;                  ///< Full function signature.
    std::string schemaName;                 ///< Schema name for the function.
    std::shared_ptr<ASTNode> astBody;       ///< AST for user-defined functions (nullptr for built-in).

    /**
     * @brief Constructs a FunctionInfo object.
     * @param category_ Function category.
     * @param name_ Function name.
     * @param returnType_ Return type.
     * @param argumentTypes_ List of argument types.
     * @param isAggregate_ Is aggregate function.
     * @param isWindow_ Is window function.
     * @param isDeterministic_ Is deterministic.
     * @param isUserDefined_ Is user-defined function.
     * @param signature_ Function signature.
     * @param schemaName_ Schema name.
     * @param astBody_ AST body for user-defined function (nullptr for built-in).
     */
    FunctionInfo(
        FunctionCategory category_ = FunctionCategory::UNKNOWN,
        std::string name_ = "",
        std::string returnType_ = "",
        std::vector<std::string> argumentTypes_ = {},
        bool isAggregate_ = false,
        bool isWindow_ = false,
        bool isDeterministic_ = true,
        bool isUserDefined_ = false,
        std::string signature_ = "",
        std::string schemaName_ = "",
        std::shared_ptr<ASTNode> astBody_ = nullptr
    )
        : category(category_), name(std::move(name_)), returnType(std::move(returnType_)),
        argumentTypes(std::move(argumentTypes_)), isAggregate(isAggregate_),
        isWindow(isWindow_), isDeterministic(isDeterministic_),
        isUserDefined(isUserDefined_), signature(std::move(signature_)),
        schemaName(std::move(schemaName_)), astBody(std::move(astBody_)) {
    }

    FunctionInfo(const FunctionInfo&) = default;
    FunctionInfo(FunctionInfo&&) = default;
    FunctionInfo& operator=(const FunctionInfo&) = default;
    FunctionInfo& operator=(FunctionInfo&&) = default;
    ~FunctionInfo() = default;
};