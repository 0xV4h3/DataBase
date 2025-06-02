/**
 * @file FloatLiteralValue.hpp
 * @brief Definition of FloatLiteralValue class.
 * @details
 * Represents a floating-point literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class FloatLiteralValue
  * @brief Represents a floating-point literal value.
  * @details
  * Provides floating-point specific operations, validation, and comparison.
  */
class FloatLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a FloatLiteralValue.
     * @param v The floating-point value
     */
    explicit FloatLiteralValue(double v);

    /**
     * @brief Default constructor (zero value).
     */
    FloatLiteralValue() : value(0.0) {}

    // Rule of five for proper memory management
    FloatLiteralValue(const FloatLiteralValue&) = default;
    FloatLiteralValue(FloatLiteralValue&&) noexcept = default;
    FloatLiteralValue& operator=(const FloatLiteralValue&) = default;
    FloatLiteralValue& operator=(FloatLiteralValue&&) noexcept = default;
    ~FloatLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the value is valid.
     * @return true if value is a valid float (not NaN or Inf)
     */
    bool isValid() const override;

    /**
     * @brief Validates the value.
     * @throws std::invalid_argument if value is NaN or Inf
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;

    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    double value; ///< The floating-point value

protected:
    /**
     * @brief Helper to check for infinity or NaN results.
     * @param result Result to check
     * @throws std::runtime_error if result is Inf or NaN
     */
    static void checkValidResult(double result);
};