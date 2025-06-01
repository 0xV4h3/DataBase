#pragma once
#include <string>
#include "TokenEnums.hpp"

/**
 * @file PunctuatorInfo.hpp
 * @brief Definition of the PunctuatorInfo class for SQL punctuator metadata.
 * @details
 * Stores metadata for a SQL punctuator or symbol, including its lexeme, category, and sub-kind (enum value).
 */

 /**
  * @class PunctuatorInfo
  * @brief Stores metadata for a SQL punctuator or symbol.
  * @details
  * Contains the text (lexeme), category (such as COMMON, STRING_DELIM, etc.), and sub-kind (specific enum value)
  * for a parsed SQL punctuator or symbol.
  *
  * @see PunctuatorCategory
  */
class PunctuatorInfo {
public:
    std::string lexeme;              ///< The punctuator text (lexeme)
    PunctuatorCategory category;     ///< Category of the punctuator (COMMON, STRING_DELIM, etc.)
    int subKind;                     ///< Sub-kind (specific enum value, e.g., CommonSymbol::COMMA)

    /**
     * @brief Constructs a PunctuatorInfo object.
     * @param lex Punctuator text (lexeme).
     * @param cat Punctuator category.
     * @param sub Sub-kind (enum value).
     */
    PunctuatorInfo(std::string lex, PunctuatorCategory cat, int sub)
        : lexeme(std::move(lex)), category(cat), subKind(sub) {
    }

    /**
     * @brief Constructs an empty PunctuatorInfo object.
     */
    PunctuatorInfo() : lexeme(), category(PunctuatorCategory::UNKNOWN), subKind(0) {}
};