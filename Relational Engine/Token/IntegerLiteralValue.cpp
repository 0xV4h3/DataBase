/**
 * @file IntegerLiteralValue.cpp
 * @brief Implementation of IntegerLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, arithmetic operations, bitwise operations, and comparison for IntegerLiteralValue.
 */

#include "LiteralValue.hpp"
#include <stdexcept>
#include <limits>
#include <cmath>
#include <sstream>

// --- Constructor ---
IntegerLiteralValue::IntegerLiteralValue(int64_t v) : value(v) {}

// --- Return as string ---
std::string IntegerLiteralValue::toString() const {
    return std::to_string(value);
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> IntegerLiteralValue::clone() const {
    return std::make_unique<IntegerLiteralValue>(value);
}

// --- Arithmetic operations ---
std::unique_ptr<LiteralValue> IntegerLiteralValue::applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const {
    // Integer + Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        switch (op) {
        case ArithmeticOp::PLUS:
            return std::make_unique<IntegerLiteralValue>(value + r->value);
        case ArithmeticOp::MINUS:
            return std::make_unique<IntegerLiteralValue>(value - r->value);
        case ArithmeticOp::MULTIPLY:
            return std::make_unique<IntegerLiteralValue>(value * r->value);
        case ArithmeticOp::DIVIDE:
            if (r->value == 0) throw std::runtime_error("Division by zero");
            return std::make_unique<IntegerLiteralValue>(value / r->value);
        case ArithmeticOp::MOD:
            if (r->value == 0) throw std::runtime_error("Modulo by zero");
            return std::make_unique<IntegerLiteralValue>(value % r->value);
        default:
            return nullptr;
        }
    }
    // Integer + Float => Float (SQL promotes)
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        double l = static_cast<double>(value);
        double rV = r->value;
        switch (op) {
        case ArithmeticOp::PLUS:
            return std::make_unique<FloatLiteralValue>(l + rV);
        case ArithmeticOp::MINUS:
            return std::make_unique<FloatLiteralValue>(l - rV);
        case ArithmeticOp::MULTIPLY:
            return std::make_unique<FloatLiteralValue>(l * rV);
        case ArithmeticOp::DIVIDE:
            if (rV == 0.0) throw std::runtime_error("Division by zero");
            return std::make_unique<FloatLiteralValue>(l / rV);
        case ArithmeticOp::MOD:
            if (rV == 0.0) throw std::runtime_error("Modulo by zero");
            return std::make_unique<FloatLiteralValue>(std::fmod(l, rV));
        default:
            return nullptr;
        }
    }
    return nullptr;
}

// --- Bitwise operations ---
std::unique_ptr<LiteralValue> IntegerLiteralValue::applyBitwise(const LiteralValue& rhs, BitwiseOp op) const {
    // Integer & Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        switch (op) {
        case BitwiseOp::BITWISE_AND:
            return std::make_unique<IntegerLiteralValue>(value & r->value);
        case BitwiseOp::BITWISE_OR:
            return std::make_unique<IntegerLiteralValue>(value | r->value);
        case BitwiseOp::BITWISE_XOR:
            return std::make_unique<IntegerLiteralValue>(value ^ r->value);
        case BitwiseOp::LEFT_SHIFT:
            return std::make_unique<IntegerLiteralValue>(value << r->value);
        case BitwiseOp::RIGHT_SHIFT:
            return std::make_unique<IntegerLiteralValue>(value >> r->value);
        default:
            return nullptr;
        }
    }
    // Unary NOT (~)
    if (op == BitwiseOp::BITWISE_NOT) {
        return std::make_unique<IntegerLiteralValue>(~value);
    }
    return nullptr;
}

// --- Comparison operators ---
bool IntegerLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // Integer <=> Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:                 return value < r->value;
        case ComparisonOp::GREATER:              return value > r->value;
        case ComparisonOp::LESS_EQUAL:           return value <= r->value;
        case ComparisonOp::GREATER_EQUAL:        return value >= r->value;
        case ComparisonOp::NOT_EQUAL:            return value != r->value;
        case ComparisonOp::EQUAL:                return value == r->value;
        default:                                 return false;
        }
    }
    // Integer <=> Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        double l = static_cast<double>(value);
        double rV = r->value;
        switch (op) {
        case ComparisonOp::LESS:                 return l < rV;
        case ComparisonOp::GREATER:              return l > rV;
        case ComparisonOp::LESS_EQUAL:           return l <= rV;
        case ComparisonOp::GREATER_EQUAL:        return l >= rV;
        case ComparisonOp::NOT_EQUAL:            return l != rV;
        case ComparisonOp::EQUAL:                return l == rV;
        default:                                 return false;
        }
    }
    return false;
}