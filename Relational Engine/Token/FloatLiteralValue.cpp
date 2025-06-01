/**
 * @file FloatLiteralValue.cpp
 * @brief Implementation of FloatLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, arithmetic operations, and comparison for FloatLiteralValue.
 */

#include "LiteralValue.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>

 // --- Constructor ---
FloatLiteralValue::FloatLiteralValue(double v) : value(v) {}

// --- Return as string ---
std::string FloatLiteralValue::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> FloatLiteralValue::clone() const {
    return std::make_unique<FloatLiteralValue>(value);
}

// --- Arithmetic operations ---
std::unique_ptr<LiteralValue> FloatLiteralValue::applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const {
    // Float + Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        switch (op) {
        case ArithmeticOp::PLUS:
            return std::make_unique<FloatLiteralValue>(value + r->value);
        case ArithmeticOp::MINUS:
            return std::make_unique<FloatLiteralValue>(value - r->value);
        case ArithmeticOp::MULTIPLY:
            return std::make_unique<FloatLiteralValue>(value * r->value);
        case ArithmeticOp::DIVIDE:
            if (r->value == 0.0)
                throw std::runtime_error("Division by zero");
            return std::make_unique<FloatLiteralValue>(value / r->value);
        case ArithmeticOp::MOD:
            if (r->value == 0.0)
                throw std::runtime_error("Modulo by zero");
            return std::make_unique<FloatLiteralValue>(std::fmod(value, r->value));
        default:
            return nullptr;
        }
    }
    // Float + Integer => Float
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        double rV = static_cast<double>(r->value);
        switch (op) {
        case ArithmeticOp::PLUS:
            return std::make_unique<FloatLiteralValue>(value + rV);
        case ArithmeticOp::MINUS:
            return std::make_unique<FloatLiteralValue>(value - rV);
        case ArithmeticOp::MULTIPLY:
            return std::make_unique<FloatLiteralValue>(value * rV);
        case ArithmeticOp::DIVIDE:
            if (rV == 0.0)
                throw std::runtime_error("Division by zero");
            return std::make_unique<FloatLiteralValue>(value / rV);
        case ArithmeticOp::MOD:
            if (rV == 0.0)
                throw std::runtime_error("Modulo by zero");
            return std::make_unique<FloatLiteralValue>(std::fmod(value, rV));
        default:
            return nullptr;
        }
    }
    return nullptr;
}

// --- Comparison operators ---
bool FloatLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // Float <=> Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:           return value < r->value;
        case ComparisonOp::GREATER:        return value > r->value;
        case ComparisonOp::LESS_EQUAL:     return value <= r->value;
        case ComparisonOp::GREATER_EQUAL:  return value >= r->value;
        case ComparisonOp::NOT_EQUAL:      return value != r->value;
        case ComparisonOp::EQUAL:          return value == r->value;
        default:                           return false;
        }
    }
    // Float <=> Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        double rV = static_cast<double>(r->value);
        switch (op) {
        case ComparisonOp::LESS:           return value < rV;
        case ComparisonOp::GREATER:        return value > rV;
        case ComparisonOp::LESS_EQUAL:     return value <= rV;
        case ComparisonOp::GREATER_EQUAL:  return value >= rV;
        case ComparisonOp::NOT_EQUAL:      return value != rV;
        case ComparisonOp::EQUAL:          return value == rV;
        default:                           return false;
        }
    }
    return false;
}