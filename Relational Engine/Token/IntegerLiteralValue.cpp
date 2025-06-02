/**
 * @file IntegerLiteralValue.cpp
 * @brief Implementation of IntegerLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, arithmetic operations,
 * bitwise operations, validation, and comparison for IntegerLiteralValue.
 */

#include "IntegerLiteralValue.hpp"
#include "FloatLiteralValue.hpp"
#include <stdexcept>
#include <limits>
#include <cmath>
#include <sstream>

 // === Constructor ===

IntegerLiteralValue::IntegerLiteralValue(int64_t v) : value(v) {
    validate();
}

// === Core Interface ===

std::string IntegerLiteralValue::toString() const {
    return std::to_string(value);
}

std::unique_ptr<LiteralValue> IntegerLiteralValue::clone() const {
    return std::make_unique<IntegerLiteralValue>(value);
}

// === Validation ===

void IntegerLiteralValue::checkOverflow(int64_t a, int64_t b, ArithmeticOp op) {
    switch (op) {
    case ArithmeticOp::PLUS:
        if ((b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
            (b < 0 && a < std::numeric_limits<int64_t>::min() - b)) {
            throw std::overflow_error("Integer addition overflow");
        }
        break;

    case ArithmeticOp::MINUS:
        if ((b < 0 && a > std::numeric_limits<int64_t>::max() + b) ||
            (b > 0 && a < std::numeric_limits<int64_t>::min() + b)) {
            throw std::overflow_error("Integer subtraction overflow");
        }
        break;

    case ArithmeticOp::MULTIPLY:
        if (a > 0) {
            if (b > 0 && a > std::numeric_limits<int64_t>::max() / b) {
                throw std::overflow_error("Integer multiplication overflow");
            }
            if (b < 0 && b < std::numeric_limits<int64_t>::min() / a) {
                throw std::overflow_error("Integer multiplication overflow");
            }
        }
        else if (a < 0) {
            if (b > 0 && a < std::numeric_limits<int64_t>::min() / b) {
                throw std::overflow_error("Integer multiplication overflow");
            }
            if (b < 0 && b < std::numeric_limits<int64_t>::max() / a) {
                throw std::overflow_error("Integer multiplication overflow");
            }
        }
        break;

    default:
        break; // Other operations can't overflow
    }
}

bool IntegerLiteralValue::equals(const LiteralValue& other) const {
    const auto* intOther = dynamic_cast<const IntegerLiteralValue*>(&other);
    return intOther && value == intOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> IntegerLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const {
    // Integer + Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        switch (op) {
        case ArithmeticOp::PLUS:
            checkOverflow(value, r->value, op);
            return std::make_unique<IntegerLiteralValue>(value + r->value);

        case ArithmeticOp::MINUS:
            checkOverflow(value, r->value, op);
            return std::make_unique<IntegerLiteralValue>(value - r->value);

        case ArithmeticOp::MULTIPLY:
            checkOverflow(value, r->value, op);
            return std::make_unique<IntegerLiteralValue>(value * r->value);

        case ArithmeticOp::DIVIDE:
            if (r->value == 0) {
                throw std::runtime_error("Division by zero");
            }
            if (value == std::numeric_limits<int64_t>::min() && r->value == -1) {
                throw std::overflow_error("Integer division overflow");
            }
            return std::make_unique<IntegerLiteralValue>(value / r->value);

        case ArithmeticOp::MOD:
            if (r->value == 0) {
                throw std::runtime_error("Modulo by zero");
            }
            if (value == std::numeric_limits<int64_t>::min() && r->value == -1) {
                throw std::overflow_error("Integer modulo overflow");
            }
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
            if (rV == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            return std::make_unique<FloatLiteralValue>(l / rV);
        case ArithmeticOp::MOD:
            if (rV == 0.0) {
                throw std::runtime_error("Modulo by zero");
            }
            return std::make_unique<FloatLiteralValue>(std::fmod(l, rV));
        default:
            return nullptr;
        }
    }
    return nullptr;
}

std::unique_ptr<LiteralValue> IntegerLiteralValue::applyBitwise(
    const LiteralValue& rhs, BitwiseOp op) const {
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
            if (r->value < 0 || r->value >= 64) {
                throw std::runtime_error("Invalid shift amount");
            }
            return std::make_unique<IntegerLiteralValue>(value << r->value);
        case BitwiseOp::RIGHT_SHIFT:
            if (r->value < 0 || r->value >= 64) {
                throw std::runtime_error("Invalid shift amount");
            }
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

bool IntegerLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // Integer <=> Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::LESS:          return value < r->value;
        case ComparisonOp::GREATER:       return value > r->value;
        case ComparisonOp::LESS_EQUAL:    return value <= r->value;
        case ComparisonOp::GREATER_EQUAL: return value >= r->value;
        case ComparisonOp::NOT_EQUAL:     return value != r->value;
        case ComparisonOp::EQUAL:         return value == r->value;
        default:                          return false;
        }
    }

    // Integer <=> Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        double l = static_cast<double>(value);
        double rV = r->value;
        switch (op) {
        case ComparisonOp::LESS:          return l < rV;
        case ComparisonOp::GREATER:       return l > rV;
        case ComparisonOp::LESS_EQUAL:    return l <= rV;
        case ComparisonOp::GREATER_EQUAL: return l >= rV;
        case ComparisonOp::NOT_EQUAL:     return l != rV;
        case ComparisonOp::EQUAL:         return l == rV;
        default:                          return false;
        }
    }
    return false;
}