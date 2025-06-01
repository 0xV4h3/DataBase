#pragma once
#include <string>
#include "TokenEnums.hpp"

/**
 * @file KeywordInfo.hpp
 * @brief Definition of the KeywordInfo class for SQL keyword metadata.
 * @details
 * Stores metadata for a SQL keyword, including its lexeme, category, and sub-kind (enum value).
 */

 /**
  * @class KeywordInfo
  * @brief Stores metadata for a SQL keyword.
  * @details
  * Contains the text (lexeme), category (such as DML, DDL, etc.), and sub-kind (specific enum value)
  * for a parsed SQL keyword.
  *
  * @see KeywordCategory
  */
class KeywordInfo {
public:
    std::string lexeme;           ///< The keyword text (lexeme)
    KeywordCategory category;     ///< Category of the keyword (DML, DDL, etc.)
    int subKind;                  ///< Sub-kind (specific enum value, e.g., DMLKeyword::SELECT)

    /**
     * @brief Constructs a KeywordInfo object.
     * @param lex Keyword text (lexeme).
     * @param cat Keyword category.
     * @param sub Sub-kind (enum value).
     */
    KeywordInfo(std::string lex, KeywordCategory cat, int sub)
        : lexeme(std::move(lex)), category(cat), subKind(sub) {
    }

    /**
     * @brief Constructs an empty KeywordInfo object.
     */
    KeywordInfo() : lexeme(), category(KeywordCategory::UNKNOWN), subKind(0) {}
};