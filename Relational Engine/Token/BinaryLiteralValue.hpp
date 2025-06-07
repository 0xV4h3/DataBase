/**
 * @file BinaryLiteralValue.hpp
 * @brief Definition of BinaryLiteralValue class.
 * @details
 * Represents a binary literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <vector>
#include <cstdint>

 /**
  * @class BinaryLiteralValue
  * @brief Represents a binary literal value.
  * @details
  * Provides binary-specific operations, validation, and comparison.
  */
class BinaryLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a BinaryLiteralValue.
     * @param v The binary value as vector of bytes
     */
    explicit BinaryLiteralValue(const std::vector<uint8_t>& v);

    /**
     * @brief Constructs a BinaryLiteralValue from a string of bits.
     * @param bitString The binary value as a string of '0' and '1' characters
     * @throws std::invalid_argument if the string contains non-binary characters
     */
    explicit BinaryLiteralValue(const std::string& bitString);
    
    /**
     * @brief Default constructor (empty binary).
     */
    BinaryLiteralValue() : value() {}

    // Rule of five for proper memory management
    BinaryLiteralValue(const BinaryLiteralValue&) = default;
    BinaryLiteralValue(BinaryLiteralValue&&) noexcept = default;
    BinaryLiteralValue& operator=(const BinaryLiteralValue&) = default;
    BinaryLiteralValue& operator=(BinaryLiteralValue&&) noexcept = default;
    ~BinaryLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the binary value is valid.
     * @return true if value is not too large
     */
    bool isValid() const override;

    /**
     * @brief Validates the binary value.
     * @throws std::invalid_argument if value exceeds maximum size
     */
    void validate() const override;

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Compares with another binary value.
     * @param rhs Right-hand binary value
     * @param op Comparison operator
     * @return Result of comparison
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    /**
     * @brief Converts a string of bits to a vector of bytes.
     * @param bitString The binary value as a string of '0' and '1' characters
     * @return Vector of bytes representing the binary value
     * @throws std::invalid_argument if the string contains non-binary characters
     */
    static std::vector<uint8_t> bitsToBytes(const std::string& bitString);

public:
    std::vector<uint8_t> value; ///< The binary value

protected:
    static constexpr size_t MAX_BINARY_SIZE = 1024 * 1024; // 1MB limit

    /**
     * @brief Formats a byte as hex string.
     * @param byte Byte to format
     * @return Formatted hex string
     */
    static std::string byteToHex(uint8_t byte);
};