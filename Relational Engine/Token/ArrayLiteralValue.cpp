/**
 * @file ArrayLiteralValue.cpp
 * @brief Implementation of ArrayLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning, concatenation, and comparison for ArrayLiteralValue.
 */

#include "LiteralValue.hpp"
#include <sstream>
#include <memory>

 // --- Constructor ---
ArrayLiteralValue::ArrayLiteralValue(std::vector<std::shared_ptr<LiteralValue>> elems)
    : elements(std::move(elems)) {
}

// --- Return as string ---
std::string ArrayLiteralValue::toString() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < elements.size(); ++i) {
        if (elements[i]) {
            oss << elements[i]->toString();
        }
        else {
            oss << "NULL";
        }
        if (i + 1 < elements.size())
            oss << ", ";
    }
    oss << "]";
    return oss.str();
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> ArrayLiteralValue::clone() const {
    std::vector<std::shared_ptr<LiteralValue>> clonedElems;
    for (const auto& elem : elements) {
        if (elem)
            clonedElems.push_back(std::shared_ptr<LiteralValue>(elem->clone().release()));
        else
            clonedElems.push_back(nullptr);
    }
    return std::make_unique<ArrayLiteralValue>(std::move(clonedElems));
}

// --- Concatenation operation ---
std::unique_ptr<LiteralValue> ArrayLiteralValue::applyConcat(const LiteralValue& rhs, ConcatOp op) const {
    if (const auto* r = dynamic_cast<const ArrayLiteralValue*>(&rhs)) {
        std::vector<std::shared_ptr<LiteralValue>> concatElems = elements;
        concatElems.insert(concatElems.end(), r->elements.begin(), r->elements.end());
        return std::make_unique<ArrayLiteralValue>(std::move(concatElems));
    }
    return nullptr;
}

// --- Compare by value ---
bool ArrayLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    if (const auto* r = dynamic_cast<const ArrayLiteralValue*>(&rhs)) {
        switch (op) {
        case ComparisonOp::EQUAL:
            return elements == r->elements;
        case ComparisonOp::NOT_EQUAL:
            return elements != r->elements;
        case ComparisonOp::LESS:
            return elements < r->elements;
        case ComparisonOp::GREATER:
            return elements > r->elements;
        case ComparisonOp::LESS_EQUAL:
            return elements <= r->elements;
        case ComparisonOp::GREATER_EQUAL:
            return elements >= r->elements;
        default:
            return false;
        }
    }
    return false;
}