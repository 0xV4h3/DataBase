/**
 * @file FunctionInfo.hpp
 * @brief Definition of the FunctionInfo class for SQL function metadata.
 * @details
 * Stores metadata about SQL functions (built-in or user-defined), including name,
 * category, arguments, return type, and properties like determinism and aggregation.
 */

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "TokenEnums.hpp"

class ASTNode;

/**
 * @class FunctionInfo
 * @brief Information about a SQL function (built-in or user-defined).
 * @details
 * Contains metadata such as function name, category, argument types, return type,
 * and various function properties. Supports user-defined functions with AST bodies.
 *
 * @see FunctionCategory
 * @see ASTNode
 */
class FunctionInfo {
public:
    // === Constructors ===

    /**
     * @brief Constructs a FunctionInfo object.
     * @param category_ Function category
     * @param name_ Function name
     * @param returnType_ Return type
     * @param argumentTypes_ List of argument types
     * @param minArgs_ Minimum number of arguments
     * @param maxArgs_ Maximum number of arguments (-1 for unlimited)
     * @param isAggregate_ Is aggregate function
     * @param isWindow_ Is window function
     * @param isDeterministic_ Is deterministic
     * @param isUserDefined_ Is user-defined function
     * @param signature_ Function signature
     * @param schemaName_ Schema name
     * @param astBody_ AST body for user-defined function (nullptr for built-in)
     * @throws std::invalid_argument if name is empty or argument counts are invalid
     */
    FunctionInfo(
        FunctionCategory category_ = FunctionCategory::UNKNOWN,
        std::string name_ = "",
        std::string returnType_ = "",
        std::vector<std::string> argumentTypes_ = {},
        int minArgs_ = 0,
        int maxArgs_ = -1,
        bool isAggregate_ = false,
        bool isWindow_ = false,
        bool isDeterministic_ = true,
        bool isUserDefined_ = false,
        std::string signature_ = "",
        std::string schemaName_ = "",
        std::shared_ptr<ASTNode> astBody_ = nullptr
    )
        : category(category_)
        , name(std::move(name_))
        , returnType(std::move(returnType_))
        , argumentTypes(std::move(argumentTypes_))
        , minArgs(minArgs_)
        , maxArgs(maxArgs_)
        , isAggregate(isAggregate_)
        , isWindow(isWindow_)
        , isDeterministic(isDeterministic_)
        , isUserDefined(isUserDefined_)
        , signature(std::move(signature_))
        , schemaName(std::move(schemaName_))
        , astBody(std::move(astBody_))
    {
        validate();
    }

    // Rule of five for proper memory management
    FunctionInfo(const FunctionInfo&) = default;
    FunctionInfo(FunctionInfo&&) noexcept = default;
    FunctionInfo& operator=(const FunctionInfo&) = default;
    FunctionInfo& operator=(FunctionInfo&&) noexcept = default;
    virtual ~FunctionInfo() = default;

    // === Accessors ===

    /**
     * @brief Gets the function category.
     * @return FunctionCategory enum value
     */
    FunctionCategory getCategory() const { return category; }

    /**
     * @brief Gets the function name.
     * @return Function name string
     */
    const std::string& getName() const { return name; }

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

    /**
     * @brief Gets the signature.
     * @return Full function signature
     */
    const std::string& getSignature() const { return signature; }

    /**
     * @brief Gets the schema name.
     * @return Schema name string
     */
    const std::string& getSchemaName() const { return schemaName; }

    // === Property Checks ===

    /**
     * @brief Checks if function accepts variable arguments.
     * @return true if function accepts variable number of arguments
     */
    bool hasVariableArgs() const { return maxArgs == -1; }

    /**
     * @brief Checks if function is aggregate.
     * @return true if function is aggregate
     */
    bool isAggregateFunction() const { return isAggregate; }

    /**
     * @brief Checks if function is window function.
     * @return true if function is window function
     */
    bool isWindowFunction() const { return isWindow; }

    /**
     * @brief Checks if function is deterministic.
     * @return true if function is deterministic
     */
    bool isDeterministicFunction() const { return isDeterministic; }

    /**
     * @brief Checks if function is user-defined.
     * @return true if function is user-defined
     */
    bool isUserDefinedFunction() const { return isUserDefined; }

    // === Validation ===

    /**
     * @brief Checks if the function info is valid.
     * @return true if required fields are valid
     */
    virtual bool isValid() const {
        return !name.empty() &&
            category != FunctionCategory::UNKNOWN &&
            minArgs >= 0 &&
            (maxArgs == -1 || maxArgs >= minArgs);
    }

    /**
     * @brief Validates the function info state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const {
        if (name.empty()) {
            throw std::invalid_argument("FunctionInfo has empty name");
        }
        if (category == FunctionCategory::UNKNOWN) {
            throw std::invalid_argument("FunctionInfo has unknown category");
        }
        if (minArgs < 0) {
            throw std::invalid_argument("FunctionInfo has invalid minimum argument count");
        }
        if (maxArgs != -1 && maxArgs < minArgs) {
            throw std::invalid_argument("FunctionInfo has invalid argument count range");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares function info objects for equality.
     * @param other FunctionInfo to compare with
     * @return true if objects are equal
     */
    virtual bool equals(const FunctionInfo& other) const {
        return name == other.name &&
            category == other.category &&
            returnType == other.returnType &&
            argumentTypes == other.argumentTypes &&
            minArgs == other.minArgs &&
            maxArgs == other.maxArgs &&
            isAggregate == other.isAggregate &&
            isWindow == other.isWindow &&
            isDeterministic == other.isDeterministic &&
            isUserDefined == other.isUserDefined &&
            signature == other.signature &&
            schemaName == other.schemaName;
    }

    /**
     * @brief Creates a string representation of the function info.
     * @return Formatted string with function details
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "FunctionInfo{name=\"" << name << "\""
            << ", category=" << static_cast<int>(category)
            << ", args=[" << minArgs << ".."
            << (maxArgs == -1 ? "∞" : std::to_string(maxArgs)) << "]"
            << ", return=\"" << returnType << "\""
            << (isAggregate ? ", aggregate" : "")
            << (isWindow ? ", window" : "")
            << (!isDeterministic ? ", non-deterministic" : "")
            << (isUserDefined ? ", user-defined" : "")
            << "}";
        return oss.str();
    }

public:
    FunctionCategory category;              ///< Function category (aggregate, scalar, etc.)
    std::string name;                       ///< Name of the function
    std::string returnType;                 ///< Return type of the function
    std::vector<std::string> argumentTypes; ///< List of argument types
    int minArgs;                           ///< Minimum number of arguments
    int maxArgs;                           ///< Maximum number of arguments (-1 for unlimited)
    bool isAggregate;                      ///< True if function is an aggregate
    bool isWindow;                         ///< True if function is a window function
    bool isDeterministic;                  ///< True if function is deterministic
    bool isUserDefined;                    ///< True if function is user-defined
    std::string signature;                 ///< Full function signature
    std::string schemaName;                ///< Schema name for the function
    std::shared_ptr<ASTNode> astBody;      ///< AST for user-defined functions (nullptr for built-in)
};