/**
 * @file UUIDLiteralValue.cpp
 * @brief Implementation of UUIDLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, comparison, validation, and generation for UUIDLiteralValue.
 */

#include "LiteralValue.hpp"
#include <uuid.h>
#include <memory>
#include <string>
#include <algorithm>
#include <random>

// --- Constructor ---
UUIDLiteralValue::UUIDLiteralValue(const std::string& v) : value(v) {}

// --- Return UUID as string ---
std::string UUIDLiteralValue::toString() const {
    return value;
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> UUIDLiteralValue::clone() const {
    return std::make_unique<UUIDLiteralValue>(value);
}

// --- Compare by parsed UUID, not text ---
bool UUIDLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const UUIDLiteralValue*>(&rhs);
    if (!r) return false;
    try {
        auto opt_a = uuids::uuid::from_string(value);
        auto opt_b = uuids::uuid::from_string(r->value);
        if (!opt_a.has_value() || !opt_b.has_value()) return false;
        const auto& uuid_a = opt_a.value();
        const auto& uuid_b = opt_b.value();
        switch (op) {
        case ComparisonOp::EQUAL:        return uuid_a == uuid_b;
        case ComparisonOp::NOT_EQUAL:    return uuid_a != uuid_b;
        case ComparisonOp::LESS:         return uuid_a < uuid_b;
        case ComparisonOp::GREATER:      return uuid_b < uuid_a;
        case ComparisonOp::LESS_EQUAL:   return !(uuid_b < uuid_a);
        case ComparisonOp::GREATER_EQUAL:return !(uuid_a < uuid_b);
        default: return false;
        }
    }
    catch (...) {
        return false;
    }
}

// --- Check UUID validity ---
bool UUIDLiteralValue::isValid() const {
    return uuids::uuid::from_string(value).has_value();
}

// --- Generate new random UUID (static) ---
UUIDLiteralValue UUIDLiteralValue::generate() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static uuids::uuid_random_generator gen{ mt };
    uuids::uuid new_uuid = gen();
    return UUIDLiteralValue(uuids::to_string(new_uuid));
}