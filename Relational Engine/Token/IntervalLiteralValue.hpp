#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <regex>

/**
 * @brief SQL INTERVAL literal supporting standard and PostgreSQL-style intervals.
 */
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
        SECOND,
        MILLISECOND,
        MICROSECOND,
        UNKNOWN
    };

    using UnitMap = std::map<Unit, int64_t>;

    // === Constructors and Destructor ===

    /**
     * @brief Constructs an IntervalLiteralValue (standard interval: INTERVAL 'val' UNIT [TO UNIT])
     */
    IntervalLiteralValue(const std::string& value,
        Unit fromUnit,
        Unit toUnit = Unit::UNKNOWN);

    /**
     * @brief Constructs a PostgreSQL-style IntervalLiteralValue (INTERVAL '2 years 3 months')
     */
    IntervalLiteralValue(const std::string& pgStyleValue);

    /**
     * @brief Default constructor (zero interval, unknown type).
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
    bool isPostgresStyle() const { return postgresStyle; }
    const UnitMap& getComponents() const { return components; }

    // === Unit Conversion/Parsing ===
    static Unit unitFromString(const std::string& unitStr);
    static std::string unitToString(Unit unit);

private:
    std::string value;      ///< The original interval value string
    Unit fromUnit;          ///< Starting unit (standard interval) or UNKNOWN (pg-style)
    Unit toUnit;            ///< Ending unit (standard interval) or UNKNOWN (pg-style)
    bool postgresStyle;     ///< True if this is a PostgreSQL-style interval
    UnitMap components;     ///< Parsed components (years, months, etc.)

    // Parsing helpers
    void parseStandard();
    void parsePostgresStyle();

    // Component helpers
    static UnitMap parsePGStyleString(const std::string& str);
    static bool validateRange(const UnitMap& components);
    int64_t toSeconds(const UnitMap& components) const;
    UnitMap fromSeconds(int64_t seconds) const;
};