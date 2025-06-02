/**
 * @file DateTimeLiteralValue.cpp
 * @brief Implementation of DateTimeLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * arithmetic operations, and comparison for DateTimeLiteralValue.
 */

#include "DateTimeLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std::chrono;

// === Constructors ===

DateTimeLiteralValue::DateTimeLiteralValue()
    : value(getCurrentDateTime())
{
    validate();
}

DateTimeLiteralValue::DateTimeLiteralValue(const std::string& v)
    : value(v)
{
    validate();
}

// === Core Interface ===

std::string DateTimeLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> DateTimeLiteralValue::clone() const {
    return std::make_unique<DateTimeLiteralValue>(value);
}

// === Protected Methods ===

std::string DateTimeLiteralValue::getCurrentDateTime() {
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    gmtime_s(&tm, &time); // Use UTC time
#else
    tm = *std::gmtime(&time); // Use UTC time
#endif
    return dateTimeToString(tm);
}

std::tm DateTimeLiteralValue::parseDateTime(const std::string& dateTimeStr) {
    std::tm tm = {};
    std::istringstream ss(dateTimeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error("Invalid datetime format: " + dateTimeStr);
    }
    return tm;
}

std::string DateTimeLiteralValue::dateTimeToString(const std::tm& tm) {
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return os.str();
}

bool DateTimeLiteralValue::validateDateTimeComponents(
    int year, int month, int day,
    int hour, int minute, int second)
{
    if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59) {
        return false;
    }

    // Check days in month
    static const int daysInMonth[] =
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int maxDays = daysInMonth[month - 1];

    // Adjust for leap year
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        maxDays = 29;
    }

    return day <= maxDays;
}

// === Validation ===

bool DateTimeLiteralValue::isValid() const {
    try {
        std::tm tm = parseDateTime(value);
        return validateDateTimeComponents(
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    catch (const std::exception&) {
        return false;
    }
}

void DateTimeLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument(
            "Invalid datetime format or values: " + value);
    }
}

bool DateTimeLiteralValue::equals(const LiteralValue& other) const {
    const auto* dtOther = dynamic_cast<const DateTimeLiteralValue*>(&other);
    return dtOther && value == dtOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> DateTimeLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const
{
    // DateTime + Integer or DateTime - Integer (seconds)
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        std::tm tm = parseDateTime(value);
        std::time_t t = std::mktime(&tm);
        if (op == ArithmeticOp::PLUS || op == ArithmeticOp::MINUS) {
            int64_t seconds = r->value;
            if (op == ArithmeticOp::MINUS) seconds = -seconds;
            t += seconds;
            std::tm newtm;
#ifdef _WIN32
            gmtime_s(&newtm, &t); // Use UTC time
#else
            newtm = *std::gmtime(&t); // Use UTC time
#endif
            return std::make_unique<DateTimeLiteralValue>(
                dateTimeToString(newtm));
        }
    }

    // DateTime - DateTime => Integer (difference in seconds)
    if (const auto* r = dynamic_cast<const DateTimeLiteralValue*>(&rhs)) {
        std::tm tm1 = parseDateTime(value);
        std::tm tm2 = parseDateTime(r->value);
        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);
        int64_t seconds = static_cast<int64_t>(std::difftime(t1, t2));
        if (op == ArithmeticOp::MINUS) {
            return std::make_unique<IntegerLiteralValue>(seconds);
        }
    }
    return nullptr;
}

bool DateTimeLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const DateTimeLiteralValue*>(&rhs)) {
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