/**
 * @file DateLiteralValue.cpp
 * @brief Implementation of DateLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * arithmetic operations, and comparison for DateLiteralValue.
 */

#include "DateLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std::chrono;

// === Constructors ===

DateLiteralValue::DateLiteralValue() : value(getCurrentDate()) {
    validate();
}

DateLiteralValue::DateLiteralValue(const std::string& v) : value(v) {
    validate();
}

// === Core Interface ===

std::string DateLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> DateLiteralValue::clone() const {
    return std::make_unique<DateLiteralValue>(value);
}

// === Protected Methods ===

std::string DateLiteralValue::getCurrentDate() {
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    tm = *std::localtime(&time);
#endif
    return dateToString(tm);
}

std::tm DateLiteralValue::parseDate(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::runtime_error("Invalid date format: " + date);
    }
    return tm;
}

std::string DateLiteralValue::dateToString(const std::tm& tm) {
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%d");
    return os.str();
}

// === Validation ===

bool DateLiteralValue::isValid() const {
    try {
        parseDate(value);
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void DateLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument("Invalid date format: " + value);
    }
}

bool DateLiteralValue::equals(const LiteralValue& other) const {
    const auto* dateOther = dynamic_cast<const DateLiteralValue*>(&other);
    return dateOther && value == dateOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> DateLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const
{
    // Date + Integer or Date - Integer (days)
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        std::tm tm = parseDate(value);
        std::time_t t = std::mktime(&tm);
        if (op == ArithmeticOp::PLUS || op == ArithmeticOp::MINUS) {
            int64_t days = r->value;
            if (op == ArithmeticOp::MINUS) days = -days;
            t += days * 24 * 60 * 60;
            std::tm newtm;
#ifdef _WIN32
            localtime_s(&newtm, &t);
#else
            newtm = *std::localtime(&t);
#endif
            return std::make_unique<DateLiteralValue>(dateToString(newtm));
        }
    }

    // Date - Date => Integer (difference in days)
    if (const auto* r = dynamic_cast<const DateLiteralValue*>(&rhs)) {
        std::tm tm1 = parseDate(value);
        std::tm tm2 = parseDate(r->value);
        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);
        int64_t days = static_cast<int64_t>(
            std::difftime(t1, t2) / (60 * 60 * 24));
        if (op == ArithmeticOp::MINUS) {
            return std::make_unique<IntegerLiteralValue>(days);
        }
    }
    return nullptr;
}

bool DateLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const DateLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:          return value < r->value;
        case ComparisonOp::GREATER:       return value > r->value;
        case ComparisonOp::LESS_EQUAL:    return value <= r->value;
        case ComparisonOp::GREATER_EQUAL: return value >= r->value;
        case ComparisonOp::NOT_EQUAL:     return value != r->value;
        case ComparisonOp::EQUAL:         return value == r->value;
        default:                          return false;
        }
    }
    return false;
}