/**
 * @file CharLiteralValue.cpp
 * @brief Implementation of CharLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, and comparison for CharLiteralValue.
 */

#include "LiteralValue.hpp"
#include <memory>
#include <string>

 // --- Constructor ---
CharLiteralValue::CharLiteralValue(char v) : value(v) {}

// --- Return as string ---
std::string CharLiteralValue::toString() const {
    return std::string(1, value);
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> CharLiteralValue::clone() const {
    return std::make_unique<CharLiteralValue>(value);
}

// --- Compare by value ---
bool CharLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const CharLiteralValue*>(&rhs);
    if (!r) return false;
    switch (op) {
    case ComparisonOp::EQUAL:         return value == r->value;
    case ComparisonOp::NOT_EQUAL:     return value != r->value;
    case ComparisonOp::LESS:          return value < r->value;
    case ComparisonOp::GREATER:       return value > r->value;
    case ComparisonOp::LESS_EQUAL:    return value <= r->value;
    case ComparisonOp::GREATER_EQUAL: return value >= r->value;
    default: return false;
    }
}