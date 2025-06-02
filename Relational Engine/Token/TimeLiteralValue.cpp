/**
 * @file TimeLiteralValue.cpp
 * @brief Implementation of TimeLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * arithmetic operations, and comparison for TimeLiteralValue.
 */

#include "TimeLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std::chrono;

// === Constructors ===

TimeLiteralValue::TimeLiteralValue() : value(getCurrentTime()) {
    validate();
}

TimeLiteralValue::TimeLiteralValue(const std::string& v) : value(v) {
    validate();
}

// === Core Interface ===

std::string TimeLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> TimeLiteralValue::clone() const {
    return std::make_unique<TimeLiteralValue>(value);
}

// === Protected Methods ===

std::string TimeLiteralValue::getCurrentTime() {
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    tm = *std::localtime(&time);
#endif
    return timeToString(tm);
}

std::tm TimeLiteralValue::parseTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error("Invalid time format: " + timeStr);
    }
    return tm;
}

std::string TimeLiteralValue::timeToString(const std::tm& tm) {
    std::ostringstream os;
    os << std::put_time(&tm, "%H:%M:%S");
    return os.str();
}

bool TimeLiteralValue::validateTimeComponents(int hours, int minutes, int seconds) {
    return hours >= 0 && hours < 24 &&
        minutes >= 0 && minutes < 60 &&
        seconds >= 0 && seconds < 60;
}

// === Validation ===

bool TimeLiteralValue::isValid() const {
    try {
        std::tm tm = parseTime(value);
        return validateTimeComponents(tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    catch (const std::exception&) {
        return false;
    }
}

void TimeLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument("Invalid time format or values: " + value);
    }
}

bool TimeLiteralValue::equals(const LiteralValue& other) const {
    const auto* timeOther = dynamic_cast<const TimeLiteralValue*>(&other);
    return timeOther && value == timeOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> TimeLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const
{
    // Time + Integer or Time - Integer (seconds)
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        std::tm tm = parseTime(value);
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
            return std::make_unique<TimeLiteralValue>(timeToString(newtm));
        }
    }

    // Time - Time => Integer (difference in seconds)
    if (const auto* r = dynamic_cast<const TimeLiteralValue*>(&rhs)) {
        std::tm tm1 = parseTime(value);
        std::tm tm2 = parseTime(r->value);
        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);
        int64_t seconds = static_cast<int64_t>(std::difftime(t1, t2));
        if (op == ArithmeticOp::MINUS) {
            return std::make_unique<IntegerLiteralValue>(seconds);
        }
    }
    return nullptr;
}

bool TimeLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const TimeLiteralValue*>(&rhs)) {
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