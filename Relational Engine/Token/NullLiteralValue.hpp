/**
 * @file NullLiteralValue.hpp
 * @brief Definition of NullLiteralValue class.
 * @details
 * Represents a SQL NULL literal value for SQL operations.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>

 /**
  * @class NullLiteralValue
  * @brief Represents a SQL NULL literal value.
  * @details
  * Provides NULL-specific operations and comparison semantics following SQL NULL rules.
  */
class NullLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Constructs a NullLiteralValue.
     */
    NullLiteralValue();

    // Rule of five for proper memory management
    NullLiteralValue(const NullLiteralValue&) = default;
    NullLiteralValue(NullLiteralValue&&) noexcept = default;
    NullLiteralValue& operator=(const NullLiteralValue&) = default;
    NullLiteralValue& operator=(NullLiteralValue&&) noexcept = default;
    ~NullLiteralValue() override = default;

    // === Core Interface ===

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===

    /**
     * @brief Checks if the NULL value is valid.
     * @return Always true as NULL is always valid
     */
    bool isValid() const override { return true; }

    /**
     * @brief Validates the NULL value.
     * No-op as NULL is always valid
     */
    void validate() const override {}

    /**
     * @brief Compares with another value for equality.
     * @param other Value to compare with
     * @return true if other is also NULL
     */
    bool equals(const LiteralValue& other) const override;

    // === Operations ===

    /**
     * @brief Compares with another value following SQL NULL comparison rules.
     * @param rhs Right-hand value
     * @param op Comparison operator
     * @return Result of comparison following SQL NULL semantics
     * @details
     * - NULL = NULL returns true
     * - NULL != any_value returns true
     * - All other comparisons return false
     */
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

protected:
    /**
     * @brief Helper to determine if operation preserves NULL.
     * @param op Comparison operator
     * @return true if the operation preserves NULL value semantics
     */
    static bool preservesNull(ComparisonOp op);
};