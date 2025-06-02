/**
 * @file DateLiteralValue.hpp
 * @brief Definition of DateLiteralValue class.
 * @details
 * Represents a date literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <ctime>

 /**
  * @class DateLiteralValue
  * @brief Represents a date literal value (YYYY-MM-DD).
  * @details
  * Provides date-specific operations, validation, arithmetic, and comparison.
  */
class DateLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a DateLiteralValue.
     * @param v The date string in YYYY-MM-DD format
     * @throws std::runtime_error if date format is invalid
     */
    explicit DateLiteralValue(const std::string& v);

    /**
     * @brief Default constructor (current date).
     */
    DateLiteralValue();

    // Rule of five for proper memory management
    DateLiteralValue(const DateLiteralValue&) = default;
    DateLiteralValue(DateLiteralValue&&) noexcept = default;
    DateLiteralValue& operator=(const DateLiteralValue&) = default;
    DateLiteralValue& operator=(DateLiteralValue&&) noexcept = default;
    ~DateLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the date string is valid.
     * @return true if date format and values are valid
     */
    bool isValid() const override;

    /**
     * @brief Validates the date string.
     * @throws std::invalid_argument if date is invalid
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if dates are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Applies arithmetic operations.
     * @param rhs Right-hand operand (Integer for +/- days, Date for difference)
     * @param op Arithmetic operator
     * @return Result of arithmetic operation
     */
    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;

    /**
     * @brief Compares with another date.
     * @param rhs Right-hand date
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    std::string value; ///< Date string in ISO format (YYYY-MM-DD)

protected:
    /**
     * @brief Parses date string to tm struct.
     * @param date Date string to parse
     * @return Parsed tm struct
     * @throws std::runtime_error if date format is invalid
     */
    static std::tm parseDate(const std::string& date);

    /**
     * @brief Converts tm struct to date string.
     * @param tm Time structure to convert
     * @return Formatted date string
     */
    static std::string dateToString(const std::tm& tm);

    /**
     * @brief Gets current date as string.
     * @return Current date in YYYY-MM-DD format
     */
    static std::string getCurrentDate();
};