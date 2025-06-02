/**
 * @file StringLiteralValue.hpp
 * @brief Definition of StringLiteralValue class.
 * @details
 * Represents a string literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class StringLiteralValue
  * @brief Represents a string literal value.
  * @details
  * Provides string-specific operations, validation, concatenation, and regex matching.
  */
class StringLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a StringLiteralValue.
     * @param v The string value
     */
    explicit StringLiteralValue(std::string v);

    /**
     * @brief Default constructor (empty string).
     */
    StringLiteralValue() : value() {}

    // Rule of five for proper memory management
    StringLiteralValue(const StringLiteralValue&) = default;
    StringLiteralValue(StringLiteralValue&&) noexcept = default;
    StringLiteralValue& operator=(const StringLiteralValue&) = default;
    StringLiteralValue& operator=(StringLiteralValue&&) noexcept = default;
    ~StringLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the string value is valid.
     * @return true if value is not empty
     */
    bool isValid() const override;

    /**
     * @brief Validates the string value.
     * @throws std::invalid_argument if value is invalid
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === String Operations ===

    /**
     * @brief Concatenates with another string.
     * @param rhs Right-hand string
     * @param op Concatenation operator (defaults to CONCAT)
     * @return Result of concatenation or nullptr if not supported
     */
    std::unique_ptr<LiteralValue> applyConcat(
        const LiteralValue& rhs, ConcatOp op = ConcatOp::CONCAT) const override;

    /**
     * @brief Compares with another string.
     * @param rhs Right-hand string
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    /**
     * @brief Applies regex pattern matching.
     * @param pattern Regex pattern
     * @param op Regex operator
     * @return Result of pattern matching
     */
    bool applyRegex(const LiteralValue& pattern, RegexOp op) const override;

public:
    std::string value; ///< The string value

protected:
    /**
     * @brief Helper to validate regex pattern.
     * @param pattern Pattern to validate
     * @return true if pattern is valid
     */
    static bool isValidRegex(const std::string& pattern);
};