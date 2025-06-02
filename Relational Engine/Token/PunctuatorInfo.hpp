/**
 * @file PunctuatorInfo.hpp
 * @brief Definition of the PunctuatorInfo class for SQL punctuator metadata.
 * @details
 * Stores metadata for SQL punctuators or symbols, including lexeme and symbol types
 * (common symbols, T-SQL specific symbols, and string delimiters).
 */

#pragma once
#include <string>
#include <sstream>
#include "TokenEnums.hpp"

 /**
  * @class PunctuatorInfo
  * @brief Stores metadata for a SQL punctuator or symbol.
  * @details
  * Contains the text (lexeme) and symbol classifications (common, T-SQL specific, or string delimiter).
  * Each punctuator can belong to multiple symbol categories simultaneously.
  *
  * @see CommonSymbol
  * @see TSQLSymbol
  * @see StringDelimiter
  */
class PunctuatorInfo {
public:
    // === Constructors ===

    /**
     * @brief Constructs a PunctuatorInfo object.
     * @param lexeme_ Punctuator text
     * @param commonSymbol_ Common symbol type
     * @param tsqlSymbol_ T-SQL symbol type
     * @param stringDelimiter_ String delimiter type
     * @throws std::invalid_argument if lexeme is empty and all types are UNKNOWN
     */
    PunctuatorInfo(
        std::string lexeme_ = "",
        CommonSymbol commonSymbol_ = CommonSymbol::UNKNOWN,
        TSQLSymbol tsqlSymbol_ = TSQLSymbol::UNKNOWN,
        StringDelimiter stringDelimiter_ = StringDelimiter::UNKNOWN
    )
        : lexeme(std::move(lexeme_))
        , commonSymbol(commonSymbol_)
        , tsqlSymbol(tsqlSymbol_)
        , stringDelimiter(stringDelimiter_)
    {
        validate();
    }

    // Rule of five for proper memory management
    PunctuatorInfo(const PunctuatorInfo&) = default;
    PunctuatorInfo(PunctuatorInfo&&) noexcept = default;
    PunctuatorInfo& operator=(const PunctuatorInfo&) = default;
    PunctuatorInfo& operator=(PunctuatorInfo&&) noexcept = default;
    virtual ~PunctuatorInfo() = default;

    // === Accessors ===

    /**
     * @brief Gets the punctuator text.
     * @return The lexeme string
     */
    const std::string& getLexeme() const { return lexeme; }

    /**
     * @brief Gets the common symbol type.
     * @return CommonSymbol enum value
     */
    CommonSymbol getCommonSymbol() const { return commonSymbol; }

    /**
     * @brief Gets the T-SQL symbol type.
     * @return TSQLSymbol enum value
     */
    TSQLSymbol getTSQLSymbol() const { return tsqlSymbol; }

    /**
     * @brief Gets the string delimiter type.
     * @return StringDelimiter enum value
     */
    StringDelimiter getStringDelimiter() const { return stringDelimiter; }

    // === Property Checks ===

    /**
     * @brief Checks if punctuator is a common symbol.
     * @return true if punctuator has a common symbol type
     */
    bool isCommonSymbol() const { return commonSymbol != CommonSymbol::UNKNOWN; }

    /**
     * @brief Checks if punctuator is a T-SQL symbol.
     * @return true if punctuator has a T-SQL symbol type
     */
    bool isTSQLSymbol() const { return tsqlSymbol != TSQLSymbol::UNKNOWN; }

    /**
     * @brief Checks if punctuator is a string delimiter.
     * @return true if punctuator has a string delimiter type
     */
    bool isStringDelimiter() const { return stringDelimiter != StringDelimiter::UNKNOWN; }

    // === Validation ===

    /**
     * @brief Checks if the punctuator info is valid.
     * @return true if lexeme is not empty and at least one symbol type is known
     */
    virtual bool isValid() const {
        return !lexeme.empty() && (
            commonSymbol != CommonSymbol::UNKNOWN ||
            tsqlSymbol != TSQLSymbol::UNKNOWN ||
            stringDelimiter != StringDelimiter::UNKNOWN
            );
    }

    /**
     * @brief Validates the punctuator info state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const {
        if (lexeme.empty()) {
            throw std::invalid_argument("PunctuatorInfo has empty lexeme");
        }
        if (commonSymbol == CommonSymbol::UNKNOWN &&
            tsqlSymbol == TSQLSymbol::UNKNOWN &&
            stringDelimiter == StringDelimiter::UNKNOWN) {
            throw std::invalid_argument("PunctuatorInfo has no valid symbol type");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares punctuator info objects for equality.
     * @param other PunctuatorInfo to compare with
     * @return true if objects are equal
     */
    virtual bool equals(const PunctuatorInfo& other) const {
        return lexeme == other.lexeme &&
            commonSymbol == other.commonSymbol &&
            tsqlSymbol == other.tsqlSymbol &&
            stringDelimiter == other.stringDelimiter;
    }

    /**
     * @brief Creates a string representation of the punctuator info.
     * @return Formatted string with punctuator details
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "PunctuatorInfo{lexeme=\"" << lexeme << "\"";
        if (isCommonSymbol()) {
            oss << ", common=" << static_cast<int>(commonSymbol);
        }
        if (isTSQLSymbol()) {
            oss << ", tsql=" << static_cast<int>(tsqlSymbol);
        }
        if (isStringDelimiter()) {
            oss << ", delim=" << static_cast<int>(stringDelimiter);
        }
        oss << "}";
        return oss.str();
    }

public:
    std::string lexeme;              ///< The punctuator text
    CommonSymbol commonSymbol;       ///< Common SQL symbol type
    TSQLSymbol tsqlSymbol;          ///< T-SQL specific symbol type
    StringDelimiter stringDelimiter; ///< String delimiter type
};