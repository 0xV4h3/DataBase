/**
 * @file NullLiteralValue.cpp
 * @brief Implementation of NullLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, and comparison for NullLiteralValue.
 */

#include "LiteralValue.hpp"
#include <memory>
#include <string>

// --- Constructor ---
NullLiteralValue::NullLiteralValue() {}

// --- Return as string ---
std::string NullLiteralValue::toString() const {
    return "NULL";
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> NullLiteralValue::clone() const {
    return std::make_unique<NullLiteralValue>();
}

// --- Compare by value ---
bool NullLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // NULL is only equal to NULL for EQUAL and NOT_EQUAL, otherwise always false.
    if (const auto* r = dynamic_cast<const NullLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::EQUAL:
            return true;
        case ComparisonOp::NOT_EQUAL:
            return false;
        default:
            return false; // NULL does not compare as less/greater/like/etc.
        }
    }
    // Comparing NULL to anything else is always false except for NOT_EQUAL (returns true)
    if (op == ComparisonOp::NOT_EQUAL)
        return true;
    return false;
}