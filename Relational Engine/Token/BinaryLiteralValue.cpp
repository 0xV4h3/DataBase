/**
 * @file BinaryLiteralValue.cpp
 * @brief Implementation of BinaryLiteralValue methods.
 * @details
 * Implements construction, string conversion, cloning,
 * validation, and comparison for BinaryLiteralValue.
 */

#include "BinaryLiteralValue.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>

 // === Constructor ===

BinaryLiteralValue::BinaryLiteralValue(const std::vector<uint8_t>& v)
    : value(v)
{
    validate();
}

BinaryLiteralValue::BinaryLiteralValue(const std::string& bitString)
    : value(bitsToBytes(bitString))
{
    validate();
}

// === Core Interface ===

std::string BinaryLiteralValue::byteToHex(uint8_t byte) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
        << static_cast<int>(byte);
    return oss.str();
}

std::string BinaryLiteralValue::toString() const {
    std::ostringstream oss;
    oss << "0x";
    for (uint8_t byte : value) {
        oss << byteToHex(byte);
    }
    return oss.str();
}

std::unique_ptr<LiteralValue> BinaryLiteralValue::clone() const {
    return std::make_unique<BinaryLiteralValue>(value);
}

// === Validation ===

bool BinaryLiteralValue::isValid() const {
    return value.size() <= MAX_BINARY_SIZE;
}

void BinaryLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument(
            "Binary value exceeds maximum size of " +
            std::to_string(MAX_BINARY_SIZE) + " bytes");
    }
}

bool BinaryLiteralValue::equals(const LiteralValue& other) const {
    const auto* binOther = dynamic_cast<const BinaryLiteralValue*>(&other);
    return binOther && value == binOther->value;
}

// === Operations ===

bool BinaryLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const BinaryLiteralValue*>(&rhs);
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

std::vector<uint8_t> BinaryLiteralValue::bitsToBytes(const std::string& bitString) {
    std::vector<uint8_t> bytes;
    size_t len = bitString.size();
    for (size_t i = 0; i < len; i += 8) {
        uint8_t byte = 0;
        int chunkSize = std::min(static_cast<size_t>(8), len - i);
        for (int j = 0; j < chunkSize; ++j) {
            if (bitString[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
            else if (bitString[i + j] != '0') {
                throw std::invalid_argument("Invalid character in binary literal: " + std::string(1, bitString[i + j]));
            }
        }
        // If the last chunk is less than 8 bits, right-align the bits (shift right)
        if (chunkSize < 8) {
            byte >>= (8 - chunkSize);
        }
        bytes.push_back(byte);
    }
    return bytes;
}