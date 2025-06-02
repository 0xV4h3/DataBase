/**
 * @file IntegerLiteralValue.hpp
 * @brief Definition of IntegerLiteralValue class.
 * @details
 * Represents an integer literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <cstdint>
#include <string>

 /**
  * @class IntegerLiteralValue
  * @brief Represents an integer literal value.
  * @details
  * Provides integer-specific operations, validation, and comparison.
  */
class IntegerLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs an IntegerLiteralValue.
     * @param v The integer value
     */
    explicit IntegerLiteralValue(int64_t v);

    /**
     * @brief Default constructor (zero value).
     */
    IntegerLiteralValue() : value(0) {}

    // Rule of five for proper memory management
    IntegerLiteralValue(const IntegerLiteralValue&) = default;
    IntegerLiteralValue(IntegerLiteralValue&&) noexcept = default;
    IntegerLiteralValue& operator=(const IntegerLiteralValue&) = default;
    IntegerLiteralValue& operator=(IntegerLiteralValue&&) noexcept = default;
    ~IntegerLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the value is valid.
     * @return true if value is in valid range
     */
    bool isValid() const override {
        return true; // All int64_t values are valid
    }

    /**
     * @brief Validates the value.
     * @throws std::invalid_argument if value is invalid
     */
    void validate() const override {}

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;

    std::unique_ptr<LiteralValue> applyBitwise(
        const LiteralValue& rhs, BitwiseOp op) const override;

    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    int64_t value; ///< The integer value

protected:
    /**
     * @brief Helper to check for overflow in arithmetic operations.
     * @param a First operand
     * @param b Second operand
     * @param op Operation type
     * @throws std::overflow_error if operation would overflow
     */
    static void checkOverflow(int64_t a, int64_t b, ArithmeticOp op);
};