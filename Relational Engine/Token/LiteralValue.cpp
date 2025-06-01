#include "LiteralValue.hpp"

/**
 * @file LiteralValue.cpp
 * @brief Default implementations for LiteralValue base class operations.
 * @details
 * All virtual methods return nullptr or false by default.
 * Subclasses override these methods to provide type-specific logic.
 */

std::unique_ptr<LiteralValue> LiteralValue::applyArithmetic(const LiteralValue&, ArithmeticOp) const {
    return nullptr;
}

std::unique_ptr<LiteralValue> LiteralValue::applyAssign(const LiteralValue&, AssignOp) const {
    return nullptr;
}

bool LiteralValue::compare(const LiteralValue&, ComparisonOp) const {
    return false;
}

std::unique_ptr<LiteralValue> LiteralValue::applyLogical(const LiteralValue&, LogicalOp) const {
    return nullptr;
}

std::unique_ptr<LiteralValue> LiteralValue::applyBitwise(const LiteralValue&, BitwiseOp) const {
    return nullptr;
}

std::unique_ptr<LiteralValue> LiteralValue::applyConcat(const LiteralValue&, ConcatOp) const {
    return nullptr;
}

std::unique_ptr<LiteralValue> LiteralValue::applyJson(const LiteralValue&, JsonOp) const {
    return nullptr;
}

bool LiteralValue::applyRegex(const LiteralValue&, RegexOp) const {
    return false;
}