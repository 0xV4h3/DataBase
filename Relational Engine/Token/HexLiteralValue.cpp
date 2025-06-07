/**
 * @file HexLiteralValue.cpp
 * @brief Implementation of HexLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * parsing, and comparison for HexLiteralValue.
 */

#include "HexLiteralValue.hpp"
#include <sstream>
#include <stdexcept>
#include <format>

 // === Constructors ===

HexLiteralValue::HexLiteralValue(uint64_t v) : value(v) {}

HexLiteralValue::HexLiteralValue(const std::string& hexStr) {
    value = parseHexString(hexStr);
}

// === Core Interface ===

std::string HexLiteralValue::toString() const {
    // 1. Produce uppercase hex with “0x” prefix
    auto hexStr = std::format("{:#X}", value);  // e.g. “0xDEADBEEF” or “0x5”

    // 2. Extract only the digits (skip “0x”)
    auto digits = hexStr.substr(2);

    // 3. If odd number of digits, pad one zero after the prefix
    if (digits.size() % 2 != 0) {
        // Re-format with one extra leading zero
        return std::format("0x0{:X}", value);
    }

    return hexStr;
}

std::unique_ptr<LiteralValue> HexLiteralValue::clone() const {
    return std::make_unique<HexLiteralValue>(value);
}

bool HexLiteralValue::equals(const LiteralValue& other) const {
    const auto* hexOther = dynamic_cast<const HexLiteralValue*>(&other);
    return hexOther && value == hexOther->value;
}

// === Operations ===

bool HexLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const HexLiteralValue*>(&rhs);
    if (!r) {
        return false;
    }

    switch (op) {
    case ComparisonOp::EQUAL:         return value == r->value;
    case ComparisonOp::NOT_EQUAL:     return value != r->value;
    case ComparisonOp::LESS:          return value < r->value;
    case ComparisonOp::GREATER:       return value > r->value;
    case ComparisonOp::LESS_EQUAL:    return value <= r->value;
    case ComparisonOp::GREATER_EQUAL: return value >= r->value;
    default:                          return false;
    }
}

// === Protected Methods ===

bool HexLiteralValue::isValidHexString(const std::string& hexStr) {
    if (hexStr.empty()) {
        return false;
    }

    size_t startPos = 0;
    if (hexStr.substr(0, 2) == "0x" || hexStr.substr(0, 2) == "0X") {
        startPos = 2;
    }

    // Check if remaining string contains only valid hex characters
    return hexStr.find_first_not_of("0123456789ABCDEFabcdef", startPos)
        == std::string::npos;
}

uint64_t HexLiteralValue::parseHexString(const std::string& hexStr) {
    if (!isValidHexString(hexStr)) {
        throw std::invalid_argument("Invalid hex string format");
    }

    std::string hexValue = hexStr;
    if (hexStr.substr(0, 2) == "0x" || hexStr.substr(0, 2) == "0X") {
        hexValue = hexStr.substr(2);
    }

    try {
        return std::stoull(hexValue, nullptr, 16);
    }
    catch (const std::exception&) {
        throw std::invalid_argument("Invalid hex value");
    }
}