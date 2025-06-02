/**
 * @file FloatLiteralValue.cpp
 * @brief Implementation of FloatLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, arithmetic operations,
 * validation, and comparison for FloatLiteralValue.
 */

#include "FloatLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <sstream>

 // === Constructor ===

FloatLiteralValue::FloatLiteralValue(double v) : value(v) {
    validate();
}

// === Core Interface ===

std::string FloatLiteralValue::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::unique_ptr<LiteralValue> FloatLiteralValue::clone() const {
    return std::make_unique<FloatLiteralValue>(value);
}

// === Validation ===

bool FloatLiteralValue::isValid() const {
    return !std::isnan(value) && !std::isinf(value);
}

void FloatLiteralValue::validate() const {
    if (std::isnan(value)) {
        throw std::invalid_argument("Float value cannot be NaN");
    }
    if (std::isinf(value)) {
        throw std::invalid_argument("Float value cannot be infinite");
    }
}

void FloatLiteralValue::checkValidResult(double result) {
    if (std::isnan(result)) {
        throw std::runtime_error("Operation resulted in NaN");
    }
    if (std::isinf(result)) {
        throw std::runtime_error("Operation resulted in infinity");
    }
}

bool FloatLiteralValue::equals(const LiteralValue& other) const {
    const auto* floatOther = dynamic_cast<const FloatLiteralValue*>(&other);
    return floatOther && value == floatOther->value;
}

// === Operations ===

std::unique_ptr<LiteralValue> FloatLiteralValue::applyArithmetic(
    const LiteralValue& rhs, ArithmeticOp op) const {
    // Float + Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
        double result;
        switch (op) {
        case ArithmeticOp::PLUS:
            result = value + r->value;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MINUS:
            result = value - r->value;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MULTIPLY:
            result = value * r->value;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::DIVIDE:
            if (r->value == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            result = value / r->value;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MOD:
            if (r->value == 0.0) {
                throw std::runtime_error("Modulo by zero");
            }
            result = std::fmod(value, r->value);
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        default:
            return nullptr;
        }
    }

    // Float + Integer => Float
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        double rV = static_cast<double>(r->value);
        double result;
        switch (op) {
        case ArithmeticOp::PLUS:
            result = value + rV;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MINUS:
            result = value - rV;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MULTIPLY:
            result = value * rV;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::DIVIDE:
            if (rV == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            result = value / rV;
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        case ArithmeticOp::MOD:
            if (rV == 0.0) {
                throw std::runtime_error("Modulo by zero");
            }
            result = std::fmod(value, rV);
            checkValidResult(result);
            return std::make_unique<FloatLiteralValue>(result);

        default:
            return nullptr;
        }
    }
    return nullptr;
}

bool FloatLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    // Float <=> Float
    if (const auto* r = dynamic_cast<const FloatLiteralValue*>(&rhs)) {
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

    // Float <=> Integer
    if (const auto* r = dynamic_cast<const IntegerLiteralValue*>(&rhs)) {
        double rV = static_cast<double>(r->value);
        switch (op) {
        case ComparisonOp::LESS:          return value < rV;
        case ComparisonOp::GREATER:       return value > rV;
        case ComparisonOp::LESS_EQUAL:    return value <= rV;
        case ComparisonOp::GREATER_EQUAL: return value >= rV;
        case ComparisonOp::NOT_EQUAL:     return value != rV;
        case ComparisonOp::EQUAL:         return value == rV;
        default:                          return false;
        }
    }
    return false;
}