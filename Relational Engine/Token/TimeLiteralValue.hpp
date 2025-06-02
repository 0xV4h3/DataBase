/**
 * @file TimeLiteralValue.hpp
 * @brief Definition of TimeLiteralValue class.
 * @details
 * Represents a time literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <ctime>

 /**
  * @class TimeLiteralValue
  * @brief Represents a time literal value (HH:MM:SS).
  * @details
  * Provides time-specific operations, validation, arithmetic, and comparison.
  */
class TimeLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a TimeLiteralValue.
     * @param v The time string in HH:MM:SS format
     * @throws std::runtime_error if time format is invalid
     */
    explicit TimeLiteralValue(const std::string& v);

    /**
     * @brief Default constructor (current time).
     */
    TimeLiteralValue();

    // Rule of five for proper memory management
    TimeLiteralValue(const TimeLiteralValue&) = default;
    TimeLiteralValue(TimeLiteralValue&&) noexcept = default;
    TimeLiteralValue& operator=(const TimeLiteralValue&) = default;
    TimeLiteralValue& operator=(TimeLiteralValue&&) noexcept = default;
    ~TimeLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the time string is valid.
     * @return true if time format and values are valid
     */
    bool isValid() const override;

    /**
     * @brief Validates the time string.
     * @throws std::invalid_argument if time is invalid
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if times are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Applies arithmetic operations.
     * @param rhs Right-hand operand (Integer for +/- seconds, Time for difference)
     * @param op Arithmetic operator
     * @return Result of arithmetic operation
     */
    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;

    /**
     * @brief Compares with another time.
     * @param rhs Right-hand time
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

public:
    std::string value; ///< Time string in ISO format (HH:MM:SS)

protected:
    /**
     * @brief Parses time string to tm struct.
     * @param timeStr Time string to parse
     * @return Parsed tm struct
     * @throws std::runtime_error if time format is invalid
     */
    static std::tm parseTime(const std::string& timeStr);

    /**
     * @brief Converts tm struct to time string.
     * @param tm Time structure to convert
     * @return Formatted time string
     */
    static std::string timeToString(const std::tm& tm);

    /**
     * @brief Gets current time as string.
     * @return Current time in HH:MM:SS format
     */
    static std::string getCurrentTime();

    /**
     * @brief Validates time components.
     * @param hours Hours value
     * @param minutes Minutes value
     * @param seconds Seconds value
     * @return true if values are valid
     */
    static bool validateTimeComponents(int hours, int minutes, int seconds);
};