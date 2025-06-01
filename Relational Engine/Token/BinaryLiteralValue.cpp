/**
 * @file BinaryLiteralValue.cpp
 * @brief Implementation of BinaryLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, and comparison for BinaryLiteralValue.
 */

#include "LiteralValue.hpp"
#include <sstream>
#include <iomanip>
#include <memory>

 // --- Constructor ---
BinaryLiteralValue::BinaryLiteralValue(const std::vector<uint8_t>& v) : value(v) {}

// --- Return as string ---
std::string BinaryLiteralValue::toString() const {
    std::ostringstream oss;
    oss << "0x";
    for (uint8_t byte : value) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> BinaryLiteralValue::clone() const {
    return std::make_unique<BinaryLiteralValue>(value);
}

// --- Compare by value ---
bool BinaryLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const BinaryLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::EQUAL:
            return value == r->value;
        case ComparisonOp::NOT_EQUAL:
            return value != r->value;
        case ComparisonOp::LESS:
            return value < r->value;
        case ComparisonOp::GREATER:
            return value > r->value;
        case ComparisonOp::LESS_EQUAL:
            return value <= r->value;
        case ComparisonOp::GREATER_EQUAL:
            return value >= r->value;
        default:
            return false;
        }
    }
    return false;
}