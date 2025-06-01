/**
 * @file DateTimeLiteralValue.cpp
 * @brief Implementation of DateTimeLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, arithmetic operations, and comparison for DateTimeLiteralValue.
 */

#include "LiteralValue.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// --- Helper: parse "YYYY-MM-DD HH:MM:SS" to std::tm ---
static std::tm parseDateTime(const std::string& dateTimeStr) {
    std::tm tm = {};
    std::istringstream ss(dateTimeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) throw std::runtime_error("Invalid datetime format: " + dateTimeStr);
    return tm;
}

// --- Helper: std::tm to "YYYY-MM-DD HH:MM:SS" string ---
static std::string dateTimeToString(const std::tm& tm) {
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return os.str();
}

// --- Constructor ---
DateTimeLiteralValue::DateTimeLiteralValue(const std::string& v) : value(v) {}

// --- Return as string ---
std::string DateTimeLiteralValue::toString() const {
    return value;
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> DateTimeLiteralValue::clone() const {
    return std::make_unique<DateTimeLiteralValue>(value);
}

// --- Arithmetic operations ---
std::unique_ptr<LiteralValue> DateTimeLiteralValue::applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const {
    // DateTime + Integer (seconds) or DateTime - Integer (seconds)
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        std::tm tm = parseDateTime(value);
        std::time_t t = std::mktime(&tm);
        if (op == ArithmeticOp::PLUS || op == ArithmeticOp::MINUS) {
            int64_t seconds = r->value;
            if (op == ArithmeticOp::MINUS) seconds = -seconds;
            t += seconds;
            std::tm newtm;
#ifdef _WIN32
            localtime_s(&newtm, &t);
#else
            newtm = *std::localtime(&t);
#endif
            return std::make_unique<DateTimeLiteralValue>(dateTimeToString(newtm));
        }
    }
    // DateTime - DateTime => Integer (difference in seconds)
    if (const auto* r = dynamic_cast<const DateTimeLiteralValue*>(&rhs)) {
        std::tm tm1 = parseDateTime(value);
        std::tm tm2 = parseDateTime(r->value);
        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);
        int64_t seconds = static_cast<int64_t>(std::difftime(t1, t2));
        if (op == ArithmeticOp::MINUS)
            return std::make_unique<IntegerLiteralValue>(seconds);
    }
    return nullptr;
}

// --- Comparison operators ---
bool DateTimeLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const DateTimeLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:
            return value < r->value;
        case ComparisonOp::GREATER:
            return value > r->value;
        case ComparisonOp::LESS_EQUAL:
            return value <= r->value;
        case ComparisonOp::GREATER_EQUAL:
            return value >= r->value;
        case ComparisonOp::NOT_EQUAL:
            return value != r->value;
        case ComparisonOp::EQUAL:
            return value == r->value;
        default:
            return false;
        }
    }
    return false;
}