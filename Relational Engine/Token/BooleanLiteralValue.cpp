/**
 * @file BooleanLiteralValue.cpp
 * @brief Implementation of BooleanLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * logical operations, and comparison for BooleanLiteralValue.
 */

#include "BooleanLiteralValue.hpp"
#include <stdexcept>

 // === Constructor ===

BooleanLiteralValue::BooleanLiteralValue(bool v) : value(v) {}

// === Core Interface ===

std::string BooleanLiteralValue::toString() const {
    return value ? "TRUE" : "FALSE";
}

std::unique_ptr<LiteralValue> BooleanLiteralValue::clone() const {
    return std::make_unique<BooleanLiteralValue>(value);
}

bool BooleanLiteralValue::equals(const LiteralValue& other) const {
    const auto* boolOther = dynamic_cast<const BooleanLiteralValue*>(&other);
    return boolOther && value == boolOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> BooleanLiteralValue::applyUnary(LogicalOp op) const {
    switch (op) {
    case LogicalOp::NOT:
        return std::make_unique<BooleanLiteralValue>(!value);
    default:
        return nullptr;
    }
}

std::unique_ptr<LiteralValue> BooleanLiteralValue::applyBinary(
    const BooleanLiteralValue& rhs, LogicalOp op) const
{
    switch (op) {
    case LogicalOp::AND:
        return std::make_unique<BooleanLiteralValue>(value && rhs.value);
    case LogicalOp::OR:
        return std::make_unique<BooleanLiteralValue>(value || rhs.value);
    case LogicalOp::XOR:
        return std::make_unique<BooleanLiteralValue>(value != rhs.value);
    default:
        return nullptr;
    }
}

std::unique_ptr<LiteralValue> BooleanLiteralValue::applyLogical(
    const LiteralValue& rhs, LogicalOp op) const
{
    // Handle unary operations first
    if (op == LogicalOp::NOT) {
        return applyUnary(op);
    }

    // Handle binary operations
    const auto* r = dynamic_cast<const BooleanLiteralValue*>(&rhs);
    if (!r) {
        return nullptr;
    }

    return applyBinary(*r, op);
}

bool BooleanLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const BooleanLiteralValue*>(&rhs);
    if (!r) {
        return false;
    }

    switch (op) {
    case ComparisonOp::EQUAL:     return value == r->value;
    case ComparisonOp::NOT_EQUAL: return value != r->value;
    default:                      return false;
    }
}