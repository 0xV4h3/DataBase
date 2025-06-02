/**
 * @file IntervalLiteralValue.cpp
 * @brief Implementation of IntervalLiteralValue methods.
 * @details
 * Implements construction, parsing, validation, arithmetic operations,
 * and comparison for IntervalLiteralValue.
 */
#include "IntervalLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include <sstream>
#include <regex>
#include <stdexcept>
#include <iomanip>

  // === Constructors ===

IntervalLiteralValue::IntervalLiteralValue()
    : value("0"), fromUnit(Unit::SECOND), toUnit(Unit::SECOND) {
}

IntervalLiteralValue::IntervalLiteralValue(
    const std::string& v, Unit from, Unit to)
    : value(v), fromUnit(from), toUnit(to)
{
    validate();
}

// === Core Interface ===

std::string IntervalLiteralValue::toString() const {
    std::ostringstream oss;
    oss << "INTERVAL '" << value << "' "
        << unitToString(fromUnit);
    if (fromUnit != toUnit) {
        oss << " TO " << unitToString(toUnit);
    }
    return oss.str();
}

std::unique_ptr<LiteralValue> IntervalLiteralValue::clone() const {
    return std::make_unique<IntervalLiteralValue>(value, fromUnit, toUnit);
}

// === Protected Methods ===

std::string IntervalLiteralValue::unitToString(Unit unit) {
    switch (unit) {
    case Unit::YEAR:   return "YEAR";
    case Unit::MONTH:  return "MONTH";
    case Unit::DAY:    return "DAY";
    case Unit::HOUR:   return "HOUR";
    case Unit::MINUTE: return "MINUTE";
    case Unit::SECOND: return "SECOND";
    default:           return "UNKNOWN";
    }
}

IntervalLiteralValue::UnitMap
IntervalLiteralValue::parseInterval(const std::string& intervalStr) const
{
    UnitMap components;

    if (fromUnit == Unit::YEAR && toUnit == Unit::MONTH) {
        std::regex rx(R"((\d+)-(\d+))");
        std::smatch match;
        if (std::regex_match(intervalStr, match, rx)) {
            components[Unit::YEAR] = std::stoll(match[1]);
            components[Unit::MONTH] = std::stoll(match[2]);
        }
        else {
            throw std::invalid_argument("Invalid YEAR TO MONTH format");
        }
    }
    else if (fromUnit == Unit::DAY && toUnit == Unit::SECOND) {
        std::regex rx(R"((\d+)\s+(\d+):(\d+):(\d+))");
        std::smatch match;
        if (std::regex_match(intervalStr, match, rx)) {
            components[Unit::DAY] = std::stoll(match[1]);
            components[Unit::HOUR] = std::stoll(match[2]);
            components[Unit::MINUTE] = std::stoll(match[3]);
            components[Unit::SECOND] = std::stoll(match[4]);
        }
        else {
            throw std::invalid_argument("Invalid DAY TO SECOND format");
        }
    }
    else {
        try {
            components[fromUnit] = std::stoll(intervalStr);
        }
        catch (const std::exception&) {
            throw std::invalid_argument("Invalid interval value");
        }
    }

    if (!validateRange(components)) {
        throw std::invalid_argument("Interval values out of range");
    }

    return components;
}

bool IntervalLiteralValue::validateRange(const UnitMap& components)
{
    for (const auto& [unit, value] : components) {
        switch (unit) {
        case Unit::MONTH:
            if (value < 0 || value > 11) return false;
            break;
        case Unit::HOUR:
            if (value < 0 || value > 23) return false;
            break;
        case Unit::MINUTE:
        case Unit::SECOND:
            if (value < 0 || value > 59) return false;
            break;
        default:
            if (value < 0) return false;
            break;
        }
    }
    return true;
}

// === Validation ===

bool IntervalLiteralValue::isValid() const {
    try {
        auto components = parseInterval(value);
        return validateRange(components);
    }
    catch (const std::exception&) {
        return false;
    }
}

void IntervalLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument(
            "Invalid interval format or values: " + value);
    }
}

bool IntervalLiteralValue::equals(const LiteralValue& other) const {
    const auto* intervalOther =
        dynamic_cast<const IntervalLiteralValue*>(&other);
    if (!intervalOther) return false;

    try {
        auto comp1 = parseInterval(value);
        auto comp2 = parseInterval(intervalOther->value);
        return toSeconds(comp1) == toSeconds(comp2);
    }
    catch (const std::exception&) {
        return false;
    }
}

// === Operations ===

int64_t IntervalLiteralValue::toSeconds(const UnitMap& components) const
{
    int64_t seconds = 0;
    for (const auto& [unit, value] : components) {
        switch (unit) {
        case Unit::YEAR:
            seconds += value * 365LL * 24 * 60 * 60;
            break;
        case Unit::MONTH:
            seconds += value * 30LL * 24 * 60 * 60;
            break;
        case Unit::DAY:
            seconds += value * 24LL * 60 * 60;
            break;
        case Unit::HOUR:
            seconds += value * 60LL * 60;
            break;
        case Unit::MINUTE:
            seconds += value * 60LL;
            break;
        case Unit::SECOND:
            seconds += value;
            break;
        }
    }
    return seconds;
}

IntervalLiteralValue::UnitMap
IntervalLiteralValue::fromSeconds(int64_t seconds) const
{
    UnitMap components;
    bool isNegative = seconds < 0;
    if (isNegative) seconds = -seconds;

    if (fromUnit == Unit::YEAR && toUnit == Unit::MONTH) {
        components[Unit::YEAR] = seconds / (365LL * 24 * 60 * 60);
        seconds %= (365LL * 24 * 60 * 60);
        components[Unit::MONTH] = (seconds / (30LL * 24 * 60 * 60)) % 12;
    }
    else if (fromUnit == Unit::DAY && toUnit == Unit::SECOND) {
        components[Unit::DAY] = seconds / (24LL * 60 * 60);
        seconds %= (24LL * 60 * 60);
        components[Unit::HOUR] = (seconds / (60LL * 60)) % 24;
        seconds %= (60LL * 60);
        components[Unit::MINUTE] = (seconds / 60LL) % 60;
        components[Unit::SECOND] = seconds % 60;
    }
    else {
        components[fromUnit] = seconds;
    }

    if (isNegative) {
        for (auto& [unit, value] : components) {
            value = -value;
        }
    }

    return components;
}

std::unique_ptr<LiteralValue> IntervalLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const
{
    const auto* r = dynamic_cast<const IntervalLiteralValue*>(&rhs);
    if (!r) return nullptr;

    try {
        auto comp1 = parseInterval(value);
        auto comp2 = parseInterval(r->value);

        int64_t sec1 = toSeconds(comp1);
        int64_t sec2 = toSeconds(comp2);

        int64_t result;
        switch (op) {
        case ArithmeticOp::PLUS:  result = sec1 + sec2; break;
        case ArithmeticOp::MINUS: result = sec1 - sec2; break;
        default: return nullptr;
        }

        auto resultComponents = fromSeconds(result);
        std::ostringstream oss;
        if (fromUnit == Unit::YEAR && toUnit == Unit::MONTH) {
            oss << std::abs(resultComponents[Unit::YEAR]) << "-"
                << std::abs(resultComponents[Unit::MONTH]);
        }
        else if (fromUnit == Unit::DAY && toUnit == Unit::SECOND) {
            oss << std::abs(resultComponents[Unit::DAY]) << " "
                << std::setw(2) << std::setfill('0')
                << std::abs(resultComponents[Unit::HOUR]) << ":"
                << std::setw(2) << std::abs(resultComponents[Unit::MINUTE]) << ":"
                << std::setw(2) << std::abs(resultComponents[Unit::SECOND]);
        }
        else {
            oss << resultComponents[fromUnit];
        }

        return std::make_unique<IntervalLiteralValue>(
            (result < 0 ? "-" : "") + oss.str(), fromUnit, toUnit);
    }
    catch (const std::exception&) {
        return nullptr;
    }
}

bool IntervalLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const IntervalLiteralValue*>(&rhs);
    if (!r) return false;

    try {
        auto comp1 = parseInterval(value);
        auto comp2 = parseInterval(r->value);

        int64_t sec1 = toSeconds(comp1);
        int64_t sec2 = toSeconds(comp2);

        switch (op) {
        case ComparisonOp::LESS:          return sec1 < sec2;
        case ComparisonOp::GREATER:       return sec1 > sec2;
        case ComparisonOp::LESS_EQUAL:    return sec1 <= sec2;
        case ComparisonOp::GREATER_EQUAL: return sec1 >= sec2;
        case ComparisonOp::NOT_EQUAL:     return sec1 != sec2;
        case ComparisonOp::EQUAL:         return sec1 == sec2;
        default:                          return false;
        }
    }
    catch (const std::exception&) {
        return false;
    }
}