/**
 * @file IdentifierInfo.hpp
 * @brief Definition of the IdentifierInfo class for SQL identifier metadata.
 * @details
 * Stores metadata about SQL identifiers (tables, columns, variables, etc.) including category,
 * name, schema, database, column list, and parameter list.
 */

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "TokenEnums.hpp"

 /**
  * @class IdentifierInfo
  * @brief Information about a SQL identifier (object, variable, etc).
  * @details
  * Contains metadata such as identifier category, name, schema, database,
  * associated columns, and parameters. Used in parsing and semantic analysis.
  *
  * @see IdentifierCategory
  */
class IdentifierInfo {
public:
    // === Constructors ===

    /**
     * @brief Constructs an IdentifierInfo object.
     * @param category_ Identifier category
     * @param name_ Identifier name
     * @param schema_ Schema name
     * @param database_ Database name
     * @param columns_ List of columns
     * @param parameters_ List of parameters
     * @throws std::invalid_argument if name is empty with non-UNKNOWN category
     */
    IdentifierInfo(
        IdentifierCategory category_ = IdentifierCategory::UNKNOWN,
        std::string name_ = "",
        std::string schema_ = "",
        std::string database_ = "",
        std::vector<std::string> columns_ = {},
        std::vector<std::string> parameters_ = {}
    )
        : category(category_)
        , name(std::move(name_))
        , schema(std::move(schema_))
        , database(std::move(database_))
        , columns(std::move(columns_))
        , parameters(std::move(parameters_))
    {
        validate();
    }

    // Rule of five for proper memory management
    IdentifierInfo(const IdentifierInfo&) = default;
    IdentifierInfo(IdentifierInfo&&) noexcept = default;
    IdentifierInfo& operator=(const IdentifierInfo&) = default;
    IdentifierInfo& operator=(IdentifierInfo&&) noexcept = default;
    virtual ~IdentifierInfo() = default;

    // === Accessors ===

    /**
     * @brief Gets the identifier category.
     * @return IdentifierCategory enum value
     */
    IdentifierCategory getCategory() const { return category; }

    /**
     * @brief Gets the identifier name.
     * @return Identifier name string
     */
    const std::string& getName() const { return name; }

    /**
     * @brief Gets the schema name.
     * @return Schema name string
     */
    const std::string& getSchema() const { return schema; }

    /**
     * @brief Gets the database name.
     * @return Database name string
     */
    const std::string& getDatabase() const { return database; }

    /**
     * @brief Gets the list of columns.
     * @return Vector of column names
     */
    const std::vector<std::string>& getColumns() const { return columns; }

    /**
     * @brief Gets the list of parameters.
     * @return Vector of parameter names
     */
    const std::vector<std::string>& getParameters() const { return parameters; }

    // === Property Checks ===

    /**
     * @brief Checks if identifier is qualified with schema.
     * @return true if identifier has schema qualification
     */
    bool isQualified() const { return !schema.empty(); }

    /**
     * @brief Checks if identifier has database context.
     * @return true if identifier has database specified
     */
    bool hasDatabase() const { return !database.empty(); }

    /**
     * @brief Checks if identifier has columns.
     * @return true if identifier has column list
     */
    bool hasColumns() const { return !columns.empty(); }

    /**
     * @brief Checks if identifier has parameters.
     * @return true if identifier has parameter list
     */
    bool hasParameters() const { return !parameters.empty(); }

    // === Validation ===

    /**
     * @brief Checks if the identifier info is valid.
     * @return true if required fields are valid
     */
    virtual bool isValid() const {
        return (category == IdentifierCategory::UNKNOWN || !name.empty()) &&
            (!isQualified() || !schema.empty()) &&
            (!hasDatabase() || !database.empty());
    }

    /**
     * @brief Validates the identifier info state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const {
        if (category != IdentifierCategory::UNKNOWN && name.empty()) {
            throw std::invalid_argument("IdentifierInfo has empty name with non-UNKNOWN category");
        }
        if (isQualified() && schema.empty()) {
            throw std::invalid_argument("IdentifierInfo claims qualification but has empty schema");
        }
        if (hasDatabase() && database.empty()) {
            throw std::invalid_argument("IdentifierInfo claims database but has empty database name");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares identifier info objects for equality.
     * @param other IdentifierInfo to compare with
     * @return true if objects are equal
     */
    virtual bool equals(const IdentifierInfo& other) const {
        return category == other.category &&
            name == other.name &&
            schema == other.schema &&
            database == other.database &&
            columns == other.columns &&
            parameters == other.parameters;
    }

    /**
     * @brief Creates a string representation of the identifier info.
     * @return Formatted string with identifier details
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "IdentifierInfo{name=\"" << name << "\"";
        if (isQualified()) {
            oss << ", schema=\"" << schema << "\"";
        }
        if (hasDatabase()) {
            oss << ", database=\"" << database << "\"";
        }
        oss << ", category=" << static_cast<int>(category);
        if (hasColumns()) {
            oss << ", columns=" << columns.size();
        }
        if (hasParameters()) {
            oss << ", params=" << parameters.size();
        }
        oss << "}";
        return oss.str();
    }

public:
    IdentifierCategory category;         ///< Identifier category (table, column, variable, etc.)
    std::string name;                    ///< Name of the identifier
    std::string schema;                  ///< Schema name, if applicable
    std::string database;                ///< Database name, if applicable
    std::vector<std::string> columns;    ///< List of columns (for tables, views, etc.)
    std::vector<std::string> parameters; ///< List of parameters (for functions, procedures, etc.)
};