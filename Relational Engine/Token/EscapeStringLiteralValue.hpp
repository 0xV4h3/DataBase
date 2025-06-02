/**
 * @file EscapeStringLiteralValue.hpp
 * @brief Definition of EscapeStringLiteralValue class.
 * @details
 * Represents an escaped string literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class EscapeStringLiteralValue
  * @brief Stores a string literal with escape sequences.
  * @details
  * Provides access to both raw (escaped) string and unescaped value,
  * with proper handling of escape sequences.
  */
class EscapeStringLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs an EscapeStringLiteralValue.
     * @param v The escaped string value
     */
    explicit EscapeStringLiteralValue(const std::string& v);

    /**
     * @brief Default constructor (empty string).
     */
    EscapeStringLiteralValue() : value() {}

    // Rule of five for proper memory management
    EscapeStringLiteralValue(const EscapeStringLiteralValue&) = default;
    EscapeStringLiteralValue(EscapeStringLiteralValue&&) noexcept = default;
    EscapeStringLiteralValue& operator=(const EscapeStringLiteralValue&) = default;
    EscapeStringLiteralValue& operator=(EscapeStringLiteralValue&&) noexcept = default;
    ~EscapeStringLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the escaped string is valid.
     * @return true if escape sequences are valid
     */
    bool isValid() const override;

    /**
     * @brief Validates the escaped string.
     * @throws std::invalid_argument if escape sequences are invalid
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal after unescaping
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Compares with another escaped string.
     * @param rhs Right-hand string
     * @param op Comparison operator
     * @return Result of comparison after unescaping
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    /**
     * @brief Returns the raw (escaped) string.
     * @return Original escaped string
     */
    std::string raw() const;

public:
    std::string value; ///< The escaped string value

protected:
    /**
     * @brief Unescapes a string, processing escape sequences.
     * @param s String to unescape
     * @return Unescaped string
     * @throws std::invalid_argument if escape sequences are invalid
     */
    static std::string unescape(const std::string& s);

    /**
     * @brief Validates escape sequences in a string.
     * @param s String to validate
     * @return true if all escape sequences are valid
     */
    static bool validateEscapes(const std::string& s);
};