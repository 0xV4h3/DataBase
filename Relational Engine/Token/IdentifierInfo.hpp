#pragma once
#include <string>
#include <vector>
#include "TokenEnums.hpp"

/**
 * @file IdentifierInfo.hpp
 * @brief Definition of the IdentifierInfo class for SQL identifier metadata.
 * @details
 * Stores metadata about SQL identifiers (tables, columns, variables, etc.) including category,
 * name, schema, database, column list, and parameter list.
 */

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
    IdentifierCategory category;         ///< Identifier category (table, column, variable, etc.)
    std::string name;                    ///< Name of the identifier.
    std::string schema;                  ///< Schema name, if applicable.
    std::string database;                ///< Database name, if applicable.
    std::vector<std::string> columns;    ///< List of columns (for tables, views, etc.)
    std::vector<std::string> parameters; ///< List of parameters (for functions, procedures, etc.)

    /**
     * @brief Constructs an IdentifierInfo object.
     * @param category_ Identifier category.
     * @param name_ Identifier name.
     * @param schema_ Schema name.
     * @param database_ Database name.
     * @param columns_ List of columns.
     * @param parameters_ List of parameters.
     */
    IdentifierInfo(
        IdentifierCategory category_ = IdentifierCategory::UNKNOWN,
        std::string name_ = "",
        std::string schema_ = "",
        std::string database_ = "",
        std::vector<std::string> columns_ = {},
        std::vector<std::string> parameters_ = {}
    )
        : category(category_), name(std::move(name_)), schema(std::move(schema_)),
        database(std::move(database_)), columns(std::move(columns_)), parameters(std::move(parameters_)) {
    }

    IdentifierInfo(const IdentifierInfo&) = default;
    IdentifierInfo(IdentifierInfo&&) = default;
    IdentifierInfo& operator=(const IdentifierInfo&) = default;
    IdentifierInfo& operator=(IdentifierInfo&&) = default;
    ~IdentifierInfo() = default;
};