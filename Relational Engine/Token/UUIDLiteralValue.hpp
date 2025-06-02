/**
 * @file UUIDLiteralValue.hpp
 * @brief Definition of UUIDLiteralValue class.
 */

#pragma once
#include "LiteralValue.hpp"
#include <uuid.h>
#include <string>
#include <optional>

class UUIDLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a UUIDLiteralValue.
     * @param v The UUID string in canonical format
     */
    explicit UUIDLiteralValue(const std::string& v);

    /**
     * @brief Default constructor (nil UUID).
     */
    UUIDLiteralValue();

    // Rule of five
    UUIDLiteralValue(const UUIDLiteralValue&) = default;
    UUIDLiteralValue(UUIDLiteralValue&&) noexcept = default;
    UUIDLiteralValue& operator=(const UUIDLiteralValue&) = default;
    UUIDLiteralValue& operator=(UUIDLiteralValue&&) noexcept = default;
    ~UUIDLiteralValue() override = default;

    // === Core Interface ===
    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===
    bool isValid() const override;
    void validate() const override;
    bool equals(const LiteralValue& other) const override;

    // === Operations ===
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // === Static Methods ===
    /**
     * @brief Generates a new random UUID.
     * @return UUIDLiteralValue containing the new UUID
     */
    static UUIDLiteralValue generate();

    /**
     * @brief Creates a nil UUID (all zeros).
     * @return UUIDLiteralValue containing nil UUID
     */
    static UUIDLiteralValue nil();

    // === Getters ===
    const std::string& getValue() const { return value; }

private:
    std::string value;  ///< Canonical UUID string

protected:
    /**
     * @brief Parses string to UUID object.
     * @return Optional containing UUID if valid
     */
    std::optional<uuids::uuid> parseUUID() const;

    /**
     * @brief Validates UUID string format.
     * @return true if format is valid
     */
    static bool validateFormat(const std::string& uuidStr);
};