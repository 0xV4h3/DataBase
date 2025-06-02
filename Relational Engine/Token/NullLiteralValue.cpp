/**
 * @file NullLiteralValue.cpp
 * @brief Implementation of NullLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * and comparison for NullLiteralValue following SQL NULL rules.
 */

#include "NullLiteralValue.hpp"

 // === Constructor ===

NullLiteralValue::NullLiteralValue() {}

// === Core Interface ===

std::string NullLiteralValue::toString() const {
    return "NULL";
}

std::unique_ptr<LiteralValue> NullLiteralValue::clone() const {
    return std::make_unique<NullLiteralValue>();
}

bool NullLiteralValue::equals(const LiteralValue& other) const {
    return dynamic_cast<const NullLiteralValue*>(&other) != nullptr;
}

// === Operations ===

bool NullLiteralValue::preservesNull(ComparisonOp op) {
    return op == ComparisonOp::EQUAL || op == ComparisonOp::NOT_EQUAL;
}

bool NullLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // Handle NULL comparison with NULL
    const auto* r = dynamic_cast<const NullLiteralValue*>(&rhs);
    if (r) {
        if (!preservesNull(op)) {
            return false; // NULL does not compare as less/greater/like/etc.
        }
        return op == ComparisonOp::EQUAL;
    }

    // Handle NULL comparison with non-NULL
    // Only NOT_EQUAL returns true, all others return false
    return op == ComparisonOp::NOT_EQUAL;
}