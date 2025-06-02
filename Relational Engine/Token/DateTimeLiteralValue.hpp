/**
 * @file DateTimeLiteralValue.hpp
 * @brief Definition of DateTimeLiteralValue class.
 * @details
 * Represents a datetime literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <ctime>

 /**
  * @class DateTimeLiteralValue
  * @brief Represents a date-time literal value (YYYY-MM-DD HH:MM:SS).
  * @details
  * Provides datetime-specific operations, validation, arithmetic, and comparison.
  */
class DateTimeLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a DateTimeLiteralValue.
     * @param v The datetime string in YYYY-MM-DD HH:MM:SS format
     * @throws std::runtime_error if datetime format is invalid
     */
    explicit DateTimeLiteralValue(const std::string& v);

    /**
     * @brief Default constructor (current UTC datetime).
     */
    DateTimeLiteralValue();

    // Rule of five for proper memory management
    DateTimeLiteralValue(const DateTimeLiteralValue&) = default;
    DateTimeLiteralValue(DateTimeLiteralValue&&) noexcept = default;
    DateTimeLiteralValue& operator=(const DateTimeLiteralValue&) = default;
    DateTimeLiteralValue& operator=(DateTimeLiteralValue&&) noexcept = default;
    ~DateTimeLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the datetime string is valid.
     * @return true if datetime format and values are valid
     */
    bool isValid() const override;

    /**
     * @brief Validates the datetime string.
     * @throws std::invalid_argument if datetime is invalid
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if datetimes are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Applies arithmetic operations.
     * @param rhs Right-hand operand (Integer for +/- seconds, DateTime for difference)
     * @param op Arithmetic operator
     * @return Result of arithmetic operation
     */
    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;

    /**
     * @brief Compares with another datetime.
     * @param rhs Right-hand datetime
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    std::string value; ///< DateTime string in ISO format (YYYY-MM-DD HH:MM:SS)

protected:
    /**
     * @brief Parses datetime string to tm struct.
     * @param dateTimeStr DateTime string to parse
     * @return Parsed tm struct
     * @throws std::runtime_error if datetime format is invalid
     */
    static std::tm parseDateTime(const std::string& dateTimeStr);

    /**
     * @brief Converts tm struct to datetime string.
     * @param tm Time structure to convert
     * @return Formatted datetime string
     */
    static std::string dateTimeToString(const std::tm& tm);

    /**
     * @brief Gets current UTC datetime as string.
     * @return Current UTC datetime in YYYY-MM-DD HH:MM:SS format
     */
    static std::string getCurrentDateTime();

    /**
     * @brief Validates datetime components.
     * @param year Year value
     * @param month Month value (1-12)
     * @param day Day value (1-31)
     * @param hour Hour value (0-23)
     * @param minute Minute value (0-59)
     * @param second Second value (0-59)
     * @return true if values are valid
     */
    static bool validateDateTimeComponents(
        int year, int month, int day,
        int hour, int minute, int second);
};