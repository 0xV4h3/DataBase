/**
 * @file BooleanLiteralValue.hpp
 * @brief Definition of BooleanLiteralValue class.
 * @details
 * Represents a boolean literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class BooleanLiteralValue
  * @brief Represents a boolean literal value.
  * @details
  * Provides boolean-specific operations, validation, logical operations, and comparison.
  */
class BooleanLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a BooleanLiteralValue.
     * @param v The boolean value
     */
    explicit BooleanLiteralValue(bool v);

    /**
     * @brief Default constructor (false value).
     */
    BooleanLiteralValue() : value(false) {}

    // Rule of five for proper memory management
    BooleanLiteralValue(const BooleanLiteralValue&) = default;
    BooleanLiteralValue(BooleanLiteralValue&&) noexcept = default;
    BooleanLiteralValue& operator=(const BooleanLiteralValue&) = default;
    BooleanLiteralValue& operator=(BooleanLiteralValue&&) noexcept = default;
    ~BooleanLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the boolean value is valid.
     * @return Always true as boolean values are always valid
     */
    bool isValid() const override { return true; }

    /**
     * @brief Validates the boolean value.
     * No-op as boolean values are always valid
     */
    void validate() const override {}

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Applies logical operations.
     * @param rhs Right-hand operand
     * @param op Logical operator
     * @return Result of logical operation
     */
    std::unique_ptr<LiteralValue> applyLogical(
        const LiteralValue& rhs, LogicalOp op) const override;

    /**
     * @brief Compares with another boolean.
     * @param rhs Right-hand boolean
     * @param op Comparison operator (only EQUAL and NOT_EQUAL supported)
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    bool value; ///< The boolean value

protected:
    /**
     * @brief Applies unary logical operation.
     * @param op Logical operator
     * @return Result of unary operation
     */
    std::unique_ptr<LiteralValue> applyUnary(LogicalOp op) const;

    /**
     * @brief Applies binary logical operation.
     * @param rhs Right-hand operand
     * @param op Logical operator
     * @return Result of binary operation
     */
    std::unique_ptr<LiteralValue> applyBinary(const BooleanLiteralValue& rhs, LogicalOp op) const;
};