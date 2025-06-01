/**
 * @file StringLiteralValue.cpp
 * @brief Implementation of StringLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, concatenation, comparison, and regex matching for StringLiteralValue.
 */

#include "LiteralValue.hpp"
#include <sstream>
#include <stdexcept>
#include <regex>

// --- Constructor ---
StringLiteralValue::StringLiteralValue(std::string v) : value(std::move(v)) {}

// --- Return as string ---
std::string StringLiteralValue::toString() const {
    return "\"" + value + "\"";
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> StringLiteralValue::clone() const {
    return std::make_unique<StringLiteralValue>(value);
}

// --- Concatenation operation ---
std::unique_ptr<LiteralValue> StringLiteralValue::applyConcat(const LiteralValue& rhs, ConcatOp op) const {
    if (op == ConcatOp::CONCAT) {
        if (const auto* r = dynamic_cast<const StringLiteralValue*>(&rhs)) {
            return std::make_unique<StringLiteralValue>(value + r->value);
        }
    }
    return nullptr;
}

// --- Compare by value ---
bool StringLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const StringLiteralValue*>(&rhs)) {
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
        case ComparisonOp::LIKE:
            return value.find(r->value) != std::string::npos;
        default:
            return false;
        }
    }
    return false;
}

// --- Regex matching ---
bool StringLiteralValue::applyRegex(const LiteralValue& pattern, RegexOp op) const {
    const auto* patternStr = dynamic_cast<const StringLiteralValue*>(&pattern);
    if (!patternStr)
        return false;

    try {
        std::regex rx(patternStr->value);
        switch (op) {
        case RegexOp::TILDE:
            return std::regex_search(value, rx);
        case RegexOp::NOT_TILDE:
            return !std::regex_search(value, rx);
        case RegexOp::TILDE_STAR:
            return std::regex_match(value, rx);
        case RegexOp::NOT_TILDE_STAR:
            return !std::regex_match(value, rx);
        default:
            return false;
        }
    }
    catch (const std::regex_error&) {
        return false;
    }
}