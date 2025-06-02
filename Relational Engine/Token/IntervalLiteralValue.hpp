/**
 * @file IntervalLiteralValue.hpp
 * @brief Definition of IntervalLiteralValue class.
 * @details
 * Represents an interval literal value for SQL operations.
 * Supports common interval units: YEAR, MONTH, DAY, HOUR, MINUTE, SECOND
 */
#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <map>
#include <cstdint>

class IntervalLiteralValue : public LiteralValue {
public:
    /**
     * @enum Unit
     * @brief Supported interval units
     */
    enum class Unit {
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        SECOND
    };

    using UnitMap = std::map<Unit, int64_t>;

    // === Constructors and Destructor ===

    /**
     * @brief Constructs an IntervalLiteralValue.
     * @param value The interval value string
     * @param fromUnit Starting unit
     * @param toUnit Ending unit (optional)
     */
    IntervalLiteralValue(const std::string& value,
        Unit fromUnit,
        Unit toUnit = Unit::SECOND);

    /**
     * @brief Default constructor (zero interval).
     */
    IntervalLiteralValue();

    // Rule of five
    IntervalLiteralValue(const IntervalLiteralValue&) = default;
    IntervalLiteralValue(IntervalLiteralValue&&) noexcept = default;
    IntervalLiteralValue& operator=(const IntervalLiteralValue&) = default;
    IntervalLiteralValue& operator=(IntervalLiteralValue&&) noexcept = default;
    ~IntervalLiteralValue() override = default;

    // === Core Interface ===
    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===
    bool isValid() const override;
    void validate() const override;
    bool equals(const LiteralValue& other) const override;

    // === Operations ===
    std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // === Getters ===
    const std::string& getValue() const { return value; }
    Unit getFromUnit() const { return fromUnit; }
    Unit getToUnit() const { return toUnit; }

private:
    std::string value;  ///< The interval value string
    Unit fromUnit;      ///< Starting unit
    Unit toUnit;        ///< Ending unit

protected:
    /**
     * @brief Parses an interval string into components.
     * @throws std::invalid_argument if format is invalid
     */
    UnitMap parseInterval(const std::string& intervalStr) const;

    /**
     * @brief Converts interval components to normalized seconds.
     */
    int64_t toSeconds(const UnitMap& components) const;

    /**
     * @brief Converts seconds to interval components.
     */
    UnitMap fromSeconds(int64_t seconds) const;

    /**
     * @brief Converts unit to string.
     */
    static std::string unitToString(Unit unit);

    /**
     * @brief Validates interval range.
     */
    static bool validateRange(const UnitMap& components);
};