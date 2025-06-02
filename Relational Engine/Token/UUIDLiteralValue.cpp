/**
 * @file UUIDLiteralValue.cpp
 * @brief Implementation of UUIDLiteralValue methods.
 */

#include "UUIDLiteralValue.hpp"
#include <random>
#include <stdexcept>
#include <regex>

 // === Constructors ===

UUIDLiteralValue::UUIDLiteralValue()
    : value("00000000-0000-0000-0000-000000000000")
{
}

UUIDLiteralValue::UUIDLiteralValue(const std::string& v)
    : value(v)
{
    validate();
}

// === Core Interface ===

std::string UUIDLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> UUIDLiteralValue::clone() const {
    return std::make_unique<UUIDLiteralValue>(value);
}

// === Protected Methods ===

std::optional<uuids::uuid> UUIDLiteralValue::parseUUID() const {
    try {
        return uuids::uuid::from_string(value);
    }
    catch (...) {
        return std::nullopt;
    }
}

bool UUIDLiteralValue::validateFormat(const std::string& uuidStr) {
    // UUID format: 8-4-4-4-12 hexadecimal digits
    static const std::regex uuidRegex(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"
    );
    return std::regex_match(uuidStr, uuidRegex);
}

// === Validation ===

bool UUIDLiteralValue::isValid() const {
    return validateFormat(value) && parseUUID().has_value();
}

void UUIDLiteralValue::validate() const {
    if (!validateFormat(value)) {
        throw std::invalid_argument(
            "Invalid UUID format: " + value);
    }
    if (!parseUUID().has_value()) {
        throw std::invalid_argument(
            "Invalid UUID value: " + value);
    }
}

bool UUIDLiteralValue::equals(const LiteralValue& other) const {
    const auto* uuidOther =
        dynamic_cast<const UUIDLiteralValue*>(&other);
    if (!uuidOther) return false;

    auto uuid1 = parseUUID();
    auto uuid2 = uuidOther->parseUUID();

    return uuid1.has_value() && uuid2.has_value() &&
        uuid1.value() == uuid2.value();
}

// === Operations ===

bool UUIDLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const UUIDLiteralValue*>(&rhs);
    if (!r) return false;

    auto uuid1 = parseUUID();
    auto uuid2 = r->parseUUID();

    if (!uuid1.has_value() || !uuid2.has_value()) {
        return false;
    }

    const auto& uuid_a = uuid1.value();
    const auto& uuid_b = uuid2.value();

    switch (op) {
    case ComparisonOp::EQUAL:         return uuid_a == uuid_b;
    case ComparisonOp::NOT_EQUAL:     return uuid_a != uuid_b;
    case ComparisonOp::LESS:          return uuid_a < uuid_b;
    case ComparisonOp::GREATER:       return uuid_b < uuid_a;
    case ComparisonOp::LESS_EQUAL:    return !(uuid_b < uuid_a);
    case ComparisonOp::GREATER_EQUAL: return !(uuid_a < uuid_b);
    default:                          return false;
    }
}

// === Static Methods ===

UUIDLiteralValue UUIDLiteralValue::generate() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static uuids::uuid_random_generator uuid_gen{ gen };

    uuids::uuid new_uuid = uuid_gen();
    return UUIDLiteralValue(uuids::to_string(new_uuid));
}

UUIDLiteralValue UUIDLiteralValue::nil() {
    return UUIDLiteralValue();
}