#include "IntervalLiteralValue.hpp"
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <map>
#include <regex>
#include <algorithm>

// --- Unit <-> String ---
IntervalLiteralValue::Unit IntervalLiteralValue::unitFromString(const std::string& unitStr) {
    std::string u = unitStr;
    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
    if (u == "YEAR" || u == "YEARS") return Unit::YEAR;
    if (u == "MONTH" || u == "MONTHS") return Unit::MONTH;
    if (u == "DAY" || u == "DAYS") return Unit::DAY;
    if (u == "HOUR" || u == "HOURS") return Unit::HOUR;
    if (u == "MINUTE" || u == "MINUTES") return Unit::MINUTE;
    if (u == "SECOND" || u == "SECONDS") return Unit::SECOND;
    if (u == "MILLISECOND" || u == "MILLISECONDS") return Unit::MILLISECOND;
    if (u == "MICROSECOND" || u == "MICROSECONDS") return Unit::MICROSECOND;
    return Unit::UNKNOWN;
}
std::string IntervalLiteralValue::unitToString(Unit unit) {
    switch (unit) {
    case Unit::YEAR: return "YEAR";
    case Unit::MONTH: return "MONTH";
    case Unit::DAY: return "DAY";
    case Unit::HOUR: return "HOUR";
    case Unit::MINUTE: return "MINUTE";
    case Unit::SECOND: return "SECOND";
    case Unit::MILLISECOND: return "MILLISECOND";
    case Unit::MICROSECOND: return "MICROSECOND";
    default: return "UNKNOWN";
    }
}

// --- Constructors ---

IntervalLiteralValue::IntervalLiteralValue()
    : value("0"), fromUnit(Unit::UNKNOWN), toUnit(Unit::UNKNOWN), postgresStyle(false) {
    components = {};
}

IntervalLiteralValue::IntervalLiteralValue(const std::string& v, Unit from, Unit to)
    : value(v), fromUnit(from), toUnit(to), postgresStyle(false) {
    parseStandard();
}

IntervalLiteralValue::IntervalLiteralValue(const std::string& pgStyleValue)
    : value(pgStyleValue), fromUnit(Unit::UNKNOWN), toUnit(Unit::UNKNOWN), postgresStyle(true) {
    parsePostgresStyle();
}

// --- Parsing ---

void IntervalLiteralValue::parseStandard() {
    components.clear();
    // Standard: single unit or YEAR TO MONTH or DAY TO SECOND, etc.
    if (fromUnit == Unit::YEAR && toUnit == Unit::MONTH) {
        // e.g. "2-3"
        std::regex rx(R"(([-+]?\d+)-(\d+))");
        std::smatch match;
        if (std::regex_match(value, match, rx)) {
            components[Unit::YEAR] = std::stoll(match[1]);
            components[Unit::MONTH] = std::stoll(match[2]);
        }
        else {
            throw std::invalid_argument("Invalid YEAR TO MONTH format: " + value);
        }
    }
    else if (fromUnit == Unit::DAY && toUnit == Unit::SECOND) {
        // e.g. "5 02:03:04"
        std::regex rx(R"(([-+]?\d+)\s+(\d+):(\d+):(\d+))");
        std::smatch match;
        if (std::regex_match(value, match, rx)) {
            components[Unit::DAY] = std::stoll(match[1]);
            components[Unit::HOUR] = std::stoll(match[2]);
            components[Unit::MINUTE] = std::stoll(match[3]);
            components[Unit::SECOND] = std::stoll(match[4]);
        }
        else {
            throw std::invalid_argument("Invalid DAY TO SECOND format: " + value);
        }
    }
    else if (fromUnit != Unit::UNKNOWN) {
        // Single unit (e.g. "2" YEAR)
        components[fromUnit] = std::stoll(value);
    }
}

void IntervalLiteralValue::parsePostgresStyle() {
    components = parsePGStyleString(value);
}

// --- PostgreSQL interval string parser ---
IntervalLiteralValue::UnitMap IntervalLiteralValue::parsePGStyleString(const std::string& str) {
    // PostgreSQL allows: [N unit] [N unit] ... (e.g. "2 years 3 months 4 days")
    static const std::vector<std::pair<std::regex, Unit>> regexps = {
        {std::regex(R"(([-+]?\d+)\s*years?)", std::regex::icase), Unit::YEAR},
        {std::regex(R"(([-+]?\d+)\s*mons?)", std::regex::icase), Unit::MONTH},
        {std::regex(R"(([-+]?\d+)\s*months?)", std::regex::icase), Unit::MONTH}, // pg allows "mon" or "month"
        {std::regex(R"(([-+]?\d+)\s*days?)", std::regex::icase), Unit::DAY},
        {std::regex(R"(([-+]?\d+)\s*hours?)", std::regex::icase), Unit::HOUR},
        {std::regex(R"(([-+]?\d+)\s*minutes?)", std::regex::icase), Unit::MINUTE},
        {std::regex(R"(([-+]?\d+)\s*mins?)", std::regex::icase), Unit::MINUTE},
        {std::regex(R"(([-+]?\d+)\s*seconds?)", std::regex::icase), Unit::SECOND},
        {std::regex(R"(([-+]?\d+)\s*milliseconds?)", std::regex::icase), Unit::MILLISECOND},
        {std::regex(R"(([-+]?\d+)\s*microseconds?)", std::regex::icase), Unit::MICROSECOND}
    };

    UnitMap out;
    std::string s = str;
    for (const auto& [rx, unit] : regexps) {
        std::smatch m;
        std::string::const_iterator searchStart(s.cbegin());
        while (std::regex_search(searchStart, s.cend(), m, rx)) {
            out[unit] += std::stoll(m[1]);
            searchStart = m.suffix().first;
        }
    }
    // Also handle time part: "hh:mm:ss"
    std::regex timeRx(R"((\d{1,2}):(\d{1,2}):(\d{1,2}))");
    std::smatch tm;
    if (std::regex_search(s, tm, timeRx)) {
        out[Unit::HOUR] += std::stoll(tm[1]);
        out[Unit::MINUTE] += std::stoll(tm[2]);
        out[Unit::SECOND] += std::stoll(tm[3]);
    }
    // If nothing was parsed, fallback to error
    if (out.empty()) throw std::invalid_argument("Unrecognized interval string: " + str);
    return out;
}

// --- toString ---
std::string IntervalLiteralValue::toString() const {
    std::ostringstream oss;
    oss << "INTERVAL '" << value << "'";
    if (!postgresStyle && fromUnit != Unit::UNKNOWN) {
        oss << " " << unitToString(fromUnit);
        if (toUnit != Unit::UNKNOWN && toUnit != fromUnit)
            oss << " TO " << unitToString(toUnit);
    }
    return oss.str();
}

std::unique_ptr<LiteralValue> IntervalLiteralValue::clone() const {
    if (postgresStyle)
        return std::make_unique<IntervalLiteralValue>(value);
    else
        return std::make_unique<IntervalLiteralValue>(value, fromUnit, toUnit);
}

// --- Validation ---
bool IntervalLiteralValue::validateRange(const UnitMap& components) {
    for (const auto& [unit, value] : components) {
        switch (unit) {
        case Unit::MONTH: if (value < 0 || value > 11) return false; break;
        case Unit::HOUR: if (value < 0 || value > 23) return false; break;
        case Unit::MINUTE: case Unit::SECOND: if (value < 0 || value > 59) return false; break;
        case Unit::MILLISECOND: if (value < 0 || value > 999) return false; break;
        case Unit::MICROSECOND: if (value < 0 || value > 999999) return false; break;
        default: if (value < 0) return false; break;
        }
    }
    return true;
}
bool IntervalLiteralValue::isValid() const {
    try {
        if (postgresStyle)
            return validateRange(components);
        else
            return validateRange(components);
    }
    catch (...) { return false; }
}
void IntervalLiteralValue::validate() const {
    if (!isValid())
        throw std::invalid_argument("Invalid interval format/values: " + value);
}
bool IntervalLiteralValue::equals(const LiteralValue& other) const {
    const auto* rhs = dynamic_cast<const IntervalLiteralValue*>(&other);
    if (!rhs) return false;
    return toSeconds(components) == toSeconds(rhs->components);
}

// --- Arithmetic/Comparison ---
int64_t IntervalLiteralValue::toSeconds(const UnitMap& cmps) const {
    int64_t seconds = 0;
    for (const auto& [unit, value] : cmps) {
        switch (unit) {
        case Unit::YEAR: seconds += value * 365LL * 24 * 60 * 60; break;
        case Unit::MONTH: seconds += value * 30LL * 24 * 60 * 60; break;
        case Unit::DAY: seconds += value * 24LL * 60 * 60; break;
        case Unit::HOUR: seconds += value * 60LL * 60; break;
        case Unit::MINUTE: seconds += value * 60LL; break;
        case Unit::SECOND: seconds += value; break;
        case Unit::MILLISECOND: seconds += value / 1000; break;
        case Unit::MICROSECOND: seconds += value / 1000000; break;
        default: break;
        }
    }
    return seconds;
}
IntervalLiteralValue::UnitMap IntervalLiteralValue::fromSeconds(int64_t seconds) const {
    UnitMap components;
    bool isNegative = seconds < 0;
    if (isNegative) seconds = -seconds;
    // Only provide basic split (years, months, days, etc.)
    components[Unit::YEAR] = seconds / (365LL * 24 * 60 * 60);
    seconds %= (365LL * 24 * 60 * 60);
    components[Unit::MONTH] = seconds / (30LL * 24 * 60 * 60);
    seconds %= (30LL * 24 * 60 * 60);
    components[Unit::DAY] = seconds / (24LL * 60 * 60);
    seconds %= (24LL * 60 * 60);
    components[Unit::HOUR] = seconds / (60LL * 60);
    seconds %= (60LL * 60);
    components[Unit::MINUTE] = seconds / 60LL;
    components[Unit::SECOND] = seconds % 60LL;
    if (isNegative) {
        for (auto& [unit, value] : components) value = -value;
    }
    return components;
}
std::unique_ptr<LiteralValue> IntervalLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const
{
    const auto* r = dynamic_cast<const IntervalLiteralValue*>(&rhs);
    if (!r) return nullptr;
    try {
        int64_t sec1 = toSeconds(this->components);
        int64_t sec2 = toSeconds(r->components);
        int64_t result;
        switch (op) {
        case ArithmeticOp::PLUS: result = sec1 + sec2; break;
        case ArithmeticOp::MINUS: result = sec1 - sec2; break;
        default: return nullptr;
        }
        auto resultComponents = fromSeconds(result);
        // Build a new PostgreSQL-style interval string for result
        std::ostringstream oss;
        if (resultComponents[Unit::YEAR]) oss << resultComponents[Unit::YEAR] << " years ";
        if (resultComponents[Unit::MONTH]) oss << resultComponents[Unit::MONTH] << " months ";
        if (resultComponents[Unit::DAY]) oss << resultComponents[Unit::DAY] << " days ";
        if (resultComponents[Unit::HOUR]) oss << resultComponents[Unit::HOUR] << " hours ";
        if (resultComponents[Unit::MINUTE]) oss << resultComponents[Unit::MINUTE] << " minutes ";
        if (resultComponents[Unit::SECOND]) oss << resultComponents[Unit::SECOND] << " seconds ";
        std::string resultStr = oss.str();
        if (!resultStr.empty() && resultStr.back() == ' ') resultStr.pop_back();
        return std::make_unique<IntervalLiteralValue>(resultStr);
    }
    catch (...) { return nullptr; }
}
bool IntervalLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const IntervalLiteralValue*>(&rhs);
    if (!r) return false;
    int64_t sec1 = toSeconds(this->components);
    int64_t sec2 = toSeconds(r->components);
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