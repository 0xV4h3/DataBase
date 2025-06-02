/**
 * @file CharLiteralValue.cpp
 * @brief Implementation of CharLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * validation, and comparison for CharLiteralValue.
 */

#include "CharLiteralValue.hpp"
#include <cctype>
#include <stdexcept>

 // === Constructor ===

CharLiteralValue::CharLiteralValue(char v) : value(v) {
    validate();
}

// === Core Interface ===

std::string CharLiteralValue::toString() const {
    return std::string(1, value);
}

std::unique_ptr<LiteralValue> CharLiteralValue::clone() const {
    return std::make_unique<CharLiteralValue>(value);
}

// === Validation ===

bool CharLiteralValue::isPrintable(char c) {
    return std::isprint(static_cast<unsigned char>(c)) != 0;
}

bool CharLiteralValue::isValid() const {
    return isPrintable(value);
}

void CharLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument("Character value must be printable");
    }
}

bool CharLiteralValue::equals(const LiteralValue& other) const {
    const auto* charOther = dynamic_cast<const CharLiteralValue*>(&other);
    return charOther && value == charOther->value;
}

// === Operations ===

bool CharLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const CharLiteralValue*>(&rhs);
    if (!r) {
        return false;
    }

    switch (op) {
    case ComparisonOp::EQUAL:         return value == r->value;
    case ComparisonOp::NOT_EQUAL:     return value != r->value;
    case ComparisonOp::LESS:          return value < r->value;
    case ComparisonOp::GREATER:       return value > r->value;
    case ComparisonOp::LESS_EQUAL:    return value <= r->value;
    case ComparisonOp::GREATER_EQUAL: return value >= r->value;
    default:                          return false;
    }
}