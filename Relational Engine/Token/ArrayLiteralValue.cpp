/**
 * @file ArrayLiteralValue.cpp
 * @brief Implementation of ArrayLiteralValue methods.
 */

#include "ArrayLiteralValue.hpp"
#include <sstream>
#include <stdexcept>

 // === Constructors ===

ArrayLiteralValue::ArrayLiteralValue()
    : elements()
{
}

ArrayLiteralValue::ArrayLiteralValue(
    std::vector<std::shared_ptr<LiteralValue>> elems)
    : elements(std::move(elems))
{
    validate();
}

// === Core Interface ===

std::string ArrayLiteralValue::toString() const {
    return elementsToString(elements);
}

std::unique_ptr<LiteralValue> ArrayLiteralValue::clone() const {
    std::vector<std::shared_ptr<LiteralValue>> clonedElems;
    clonedElems.reserve(elements.size());

    for (const auto& elem : elements) {
        if (elem) {
            clonedElems.push_back(
                std::shared_ptr<LiteralValue>(elem->clone().release()));
        }
        else {
            clonedElems.push_back(nullptr);
        }
    }

    return std::make_unique<ArrayLiteralValue>(std::move(clonedElems));
}

// === Protected Methods ===

bool ArrayLiteralValue::validateElements() const noexcept {
    for (const auto& elem : elements) {
        if (elem && !elem->isValid()) {
            return false;
        }
    }
    return true;
}

std::string ArrayLiteralValue::elementsToString(
    const std::vector<std::shared_ptr<LiteralValue>>& elems) noexcept
{
    std::ostringstream oss;
    oss << "[";

    for (size_t i = 0; i < elems.size(); ++i) {
        if (elems[i]) {
            oss << elems[i]->toString();
        }
        else {
            oss << "NULL";
        }
        if (i + 1 < elems.size()) {
            oss << ", ";
        }
    }

    oss << "]";
    return oss.str();
}

// === Validation ===

bool ArrayLiteralValue::isValid() const {
    return validateElements();
}

void ArrayLiteralValue::validate() const {
    if (!validateElements()) {
        throw std::invalid_argument("Array contains invalid elements");
    }
}

bool ArrayLiteralValue::equals(const LiteralValue& other) const {
    const auto* arrayOther =
        dynamic_cast<const ArrayLiteralValue*>(&other);
    if (!arrayOther) return false;

    if (elements.size() != arrayOther->elements.size()) {
        return false;
    }

    for (size_t i = 0; i < elements.size(); ++i) {
        if (!elements[i] && !arrayOther->elements[i]) {
            continue; // Both null
        }
        if (!elements[i] || !arrayOther->elements[i]) {
            return false; // One null, one not
        }
        if (!elements[i]->equals(*arrayOther->elements[i])) {
            return false;
        }
    }
    return true;
}

// === Operations ===

std::unique_ptr<LiteralValue> ArrayLiteralValue::applyConcat(
    const LiteralValue& rhs, ConcatOp op) const
{
    const auto* r = dynamic_cast<const ArrayLiteralValue*>(&rhs);
    if (!r) return nullptr;

    std::vector<std::shared_ptr<LiteralValue>> concatElems = elements;
    concatElems.insert(
        concatElems.end(),
        r->elements.begin(),
        r->elements.end());

    return std::make_unique<ArrayLiteralValue>(std::move(concatElems));
}

bool ArrayLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const ArrayLiteralValue*>(&rhs);
    if (!r) return false;

    switch (op) {
    case ComparisonOp::EQUAL:         return elements == r->elements;
    case ComparisonOp::NOT_EQUAL:     return elements != r->elements;
    case ComparisonOp::LESS:          return elements < r->elements;
    case ComparisonOp::GREATER:       return elements > r->elements;
    case ComparisonOp::LESS_EQUAL:    return elements <= r->elements;
    case ComparisonOp::GREATER_EQUAL: return elements >= r->elements;
    default:                          return false;
    }
}

// === Array-specific Operations ===

std::shared_ptr<LiteralValue>
ArrayLiteralValue::at(size_t index) const noexcept {
    if (index >= elements.size()) {
        return nullptr;
    }
    return elements[index];
}