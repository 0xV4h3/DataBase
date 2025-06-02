/**
 * @file StringLiteralValue.cpp
 * @brief Implementation of StringLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, concatenation,
 * comparison, validation, and regex matching for StringLiteralValue.
 */

#include "StringLiteralValue.hpp"
#include <sstream>
#include <stdexcept>
#include <regex>

 // === Constructor ===

StringLiteralValue::StringLiteralValue(std::string v)
    : value(std::move(v))
{
    validate();
}

// === Core Interface ===

std::string StringLiteralValue::toString() const {
    return "\"" + value + "\"";
}

std::unique_ptr<LiteralValue> StringLiteralValue::clone() const {
    return std::make_unique<StringLiteralValue>(value);
}

// === Validation ===

bool StringLiteralValue::isValid() const {
    return !value.empty();
}

void StringLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument("String value cannot be empty");
    }
}

bool StringLiteralValue::equals(const LiteralValue& other) const {
    const auto* strOther = dynamic_cast<const StringLiteralValue*>(&other);
    return strOther && value == strOther->value;
}

bool StringLiteralValue::isValidRegex(const std::string& pattern) {
    try {
        std::regex rx(pattern);
        return true;
    }
    catch (const std::regex_error&) {
        return false;
    }
}

// === String Operations ===

std::unique_ptr<LiteralValue> StringLiteralValue::applyConcat(
    const LiteralValue& rhs, ConcatOp op) const
{
    if (op == ConcatOp::CONCAT) {
        if (const auto* r = dynamic_cast<const StringLiteralValue*>(&rhs)) {
            return std::make_unique<StringLiteralValue>(value + r->value);
        }
    }
    return nullptr;
}

bool StringLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const StringLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:          return value < r->value;
        case ComparisonOp::GREATER:       return value > r->value;
        case ComparisonOp::LESS_EQUAL:    return value <= r->value;
        case ComparisonOp::GREATER_EQUAL: return value >= r->value;
        case ComparisonOp::NOT_EQUAL:     return value != r->value;
        case ComparisonOp::EQUAL:         return value == r->value;
        case ComparisonOp::LIKE:          return value.find(r->value) != std::string::npos;
        default:                          return false;
        }
    }
    return false;
}

bool StringLiteralValue::applyRegex(const LiteralValue& pattern, RegexOp op) const {
    const auto* patternStr = dynamic_cast<const StringLiteralValue*>(&pattern);
    if (!patternStr) {
        return false;
    }

    if (!isValidRegex(patternStr->value)) {
        return false;
    }

    try {
        std::regex rx(patternStr->value);
        switch (op) {
        case RegexOp::TILDE:          return std::regex_search(value, rx);
        case RegexOp::NOT_TILDE:      return !std::regex_search(value, rx);
        case RegexOp::TILDE_STAR:     return std::regex_match(value, rx);
        case RegexOp::NOT_TILDE_STAR: return !std::regex_match(value, rx);
        default:                      return false;
        }
    }
    catch (const std::regex_error&) {
        return false;
    }
}