/**
 * @file BooleanLiteralValue.cpp
 * @brief Implementation of BooleanLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, logical operations, and comparison for BooleanLiteralValue.
 */

#include "LiteralValue.hpp"
#include <memory>
#include <string>

 // --- Constructor ---
BooleanLiteralValue::BooleanLiteralValue(bool v) : value(v) {}

// --- Return as string ---
std::string BooleanLiteralValue::toString() const {
    return value ? "TRUE" : "FALSE";
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> BooleanLiteralValue::clone() const {
    return std::make_unique<BooleanLiteralValue>(value);
}

// --- Logical operations ---
std::unique_ptr<LiteralValue> BooleanLiteralValue::applyLogical(const LiteralValue& rhs, LogicalOp op) const {
    if (const auto* r = dynamic_cast<const BooleanLiteralValue*>(&rhs)) {
        switch (op) {
        case LogicalOp::AND:
            return std::make_unique<BooleanLiteralValue>(value && r->value);
        case LogicalOp::OR:
            return std::make_unique<BooleanLiteralValue>(value || r->value);
        case LogicalOp::XOR:
            return std::make_unique<BooleanLiteralValue>(value != r->value);
        default:
            break;
        }
    }
    if (op == LogicalOp::NOT) {
        return std::make_unique<BooleanLiteralValue>(!value);
    }
    return nullptr;
}

// --- Compare by value ---
bool BooleanLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const BooleanLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::NOT_EQUAL:
            return value != r->value;
        case ComparisonOp::EQUAL:
            return value == r->value;
        default:
            return false;
        }
    }
    return false;
}