/**
 * @file KeywordInfo.hpp
 * @brief Definition of the KeywordInfo class for SQL keyword metadata.
 * @details
 * Stores metadata for a SQL keyword, including its lexeme, category, and sub-kind (enum value).
 */

#pragma once
#include <string>
#include <sstream>
#include "TokenEnums.hpp"

 /**
  * @class KeywordInfo
  * @brief Stores metadata for a SQL keyword.
  * @details
  * Contains the text (lexeme), category (such as DML, DDL, etc.), and sub-kind (specific enum value)
  * for a parsed SQL keyword. Provides validation and comparison methods.
  *
  * @see KeywordCategory
  */
class KeywordInfo {
public:
    // === Constructors ===

    /**
     * @brief Constructs a KeywordInfo object.
     * @param lex Keyword text (lexeme)
     * @param cat Keyword category
     * @param sub Sub-kind (enum value)
     * @throws std::invalid_argument if lexeme is empty
     */
    KeywordInfo(std::string lex, KeywordCategory cat, int sub)
        : lexeme(std::move(lex)), category(cat), subKind(sub)
    {
        validate();
    }

    /**
     * @brief Constructs an empty KeywordInfo object (invalid/unknown).
     */
    KeywordInfo()
        : lexeme(), category(KeywordCategory::UNKNOWN), subKind(0)
    {
    }

    // Rule of five for proper memory management
    KeywordInfo(const KeywordInfo&) = default;
    KeywordInfo(KeywordInfo&&) noexcept = default;
    KeywordInfo& operator=(const KeywordInfo&) = default;
    KeywordInfo& operator=(KeywordInfo&&) noexcept = default;
    virtual ~KeywordInfo() = default;

    // === Accessors ===

    /**
     * @brief Gets the keyword text.
     * @return The lexeme string
     */
    const std::string& getLexeme() const {
        return lexeme;
    }

    /**
     * @brief Gets the keyword category.
     * @return KeywordCategory enum value
     */
    KeywordCategory getCategory() const {
        return category;
    }

    /**
     * @brief Gets the keyword sub-kind.
     * @return Sub-kind enum value
     */
    int getSubKind() const {
        return subKind;
    }

    // === Validation ===

    /**
     * @brief Checks if the keyword info is valid.
     * @return true if lexeme is not empty and category is known
     */
    virtual bool isValid() const {
        return !lexeme.empty() && category != KeywordCategory::UNKNOWN;
    }

    /**
     * @brief Validates the keyword info state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const {
        if (lexeme.empty()) {
            throw std::invalid_argument("KeywordInfo has empty lexeme");
        }
        if (category == KeywordCategory::UNKNOWN) {
            throw std::invalid_argument("KeywordInfo has unknown category");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares keyword info objects for equality.
     * @param other KeywordInfo to compare with
     * @return true if objects are equal
     */
    virtual bool equals(const KeywordInfo& other) const {
        return lexeme == other.lexeme &&
            category == other.category &&
            subKind == other.subKind;
    }

    /**
     * @brief Creates a string representation of the keyword info.
     * @return Formatted string with keyword details
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "KeywordInfo{lexeme=\"" << lexeme << "\""
            << ", category=" << static_cast<int>(category)
            << ", subKind=" << subKind << "}";
        return oss.str();
    }

public:
    std::string lexeme;           ///< The keyword text (lexeme)
    KeywordCategory category;     ///< Category of the keyword (DML, DDL, etc.)
    int subKind;                 ///< Sub-kind (specific enum value, e.g., DMLKeyword::SELECT)
};