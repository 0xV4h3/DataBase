/**
 * @file HexLiteralValue.hpp
 * @brief Definition of HexLiteralValue class.
 * @details
 * Represents a hexadecimal literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <cstdint>

 /**
  * @class HexLiteralValue
  * @brief Represents a hexadecimal literal value.
  * @details
  * Provides hex-specific operations, validation, and comparison.
  * Stores value as uint64_t internally but handles hex string representation.
  */
class HexLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a HexLiteralValue from uint64_t.
     * @param v The numeric value
     */
    explicit HexLiteralValue(uint64_t v);

    /**
     * @brief Constructs a HexLiteralValue from hex string.
     * @param hexStr The hex string (with or without 0x prefix)
     */
    explicit HexLiteralValue(const std::string& hexStr);

    /**
     * @brief Default constructor (zero value).
     */
    HexLiteralValue() : value(0) {}

    // Rule of five for proper memory management
    HexLiteralValue(const HexLiteralValue&) = default;
    HexLiteralValue(HexLiteralValue&&) noexcept = default;
    HexLiteralValue& operator=(const HexLiteralValue&) = default;
    HexLiteralValue& operator=(HexLiteralValue&&) noexcept = default;
    ~HexLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the hex value is valid.
     * @return Always true as any uint64_t is valid
     */
    bool isValid() const override { return true; }

    /**
     * @brief Validates the hex value.
     * No-op as any uint64_t is valid
     */
    void validate() const override {}

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Compares with another hex value.
     * @param rhs Right-hand hex value
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    /**
     * @brief Gets the raw numeric value.
     * @return The internal uint64_t value
     */
    uint64_t getValue() const { return value; }

public:
    uint64_t value; ///< The numeric value

protected:
    /**
     * @brief Converts hex string to uint64_t.
     * @param hexStr The hex string to convert
     * @return Converted numeric value
     * @throws std::invalid_argument if string is invalid hex
     */
    static uint64_t parseHexString(const std::string& hexStr);

    /**
     * @brief Validates hex string format.
     * @param hexStr The hex string to validate
     * @return true if string is valid hex
     */
    static bool isValidHexString(const std::string& hexStr);
};