/**
 * @file CharLiteralValue.hpp
 * @brief Definition of CharLiteralValue class.
 * @details
 * Represents a character literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class CharLiteralValue
  * @brief Represents a character literal value (single Unicode code unit).
  * @details
  * Provides character-specific operations, validation, and comparison.
  */
class CharLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a CharLiteralValue.
     * @param v The character value
     */
    explicit CharLiteralValue(char v);

    /**
     * @brief Default constructor (null character).
     */
    CharLiteralValue() : value('\0') {}

    // Rule of five for proper memory management
    CharLiteralValue(const CharLiteralValue&) = default;
    CharLiteralValue(CharLiteralValue&&) noexcept = default;
    CharLiteralValue& operator=(const CharLiteralValue&) = default;
    CharLiteralValue& operator=(CharLiteralValue&&) noexcept = default;
    ~CharLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the character value is valid.
     * @return true if value is printable
     */
    bool isValid() const override;

    /**
     * @brief Validates the character value.
     * @throws std::invalid_argument if value is not printable
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Compares with another character.
     * @param rhs Right-hand character
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    char value; ///< The character value

protected:
    /**
     * @brief Checks if character is printable.
     * @param c Character to check
     * @return true if character is printable
     */
    static bool isPrintable(char c);
};