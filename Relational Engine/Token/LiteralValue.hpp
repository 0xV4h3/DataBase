/**
 * @file LiteralValue.hpp
 * @brief Base class for all literal values used in tokens.
 * @details
 * Provides an abstract interface for SQL literal values with validation,
 * comparison, and operation support.
 */

#pragma once
#include <string>
#include <sstream>
#include <memory>
#include "TokenEnums.hpp"

class LiteralValue {
public:
    // === Constructors & Destructor ===

    LiteralValue() = default;

    // Rule of five for proper memory management
    LiteralValue(const LiteralValue&) = default;
    LiteralValue(LiteralValue&&) noexcept = default;
    LiteralValue& operator=(const LiteralValue&) = default;
    LiteralValue& operator=(LiteralValue&&) noexcept = default;
    virtual ~LiteralValue() = default;

    // === Core Interface ===

    /**
     * @brief Converts the literal value to a string representation.
     * @return String representation of the value
     */
    virtual std::string toString() const = 0;

    /**
     * @brief Creates a deep copy of the literal value.
     * @return Unique pointer to the cloned value
     */
    virtual std::unique_ptr<LiteralValue> clone() const = 0;

    // === Validation ===

    /**
     * @brief Checks if the literal value is valid.
     * @return true if value is in valid state
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Validates the literal value's state.
     * @throws std::invalid_argument if state is invalid
     */
    virtual void validate() const = 0;

    // === Value Operations ===

    /**
     * @brief Compares this value with another for equality.
     * @param other Value to compare with
     * @return true if values are equal
     */
    virtual bool equals(const LiteralValue& other) const = 0;

    /**
     * @brief Checks if values are exactly identical.
     * @param other Value to compare with
     * @return true if values are identical
     */
    virtual bool identical(const LiteralValue& other) const {
        return typeid(*this) == typeid(other) && equals(other);
    }

    // === Arithmetic Operations ===

    /**
     * @brief Applies arithmetic operation with another value.
     * @param rhs Right-hand operand
     * @param op Arithmetic operation to perform
     * @return Result of operation or nullptr if not supported
     */
    virtual std::unique_ptr<LiteralValue> applyArithmetic(
        const LiteralValue& rhs, ArithmeticOp op) const {
        return nullptr;
    }

    // === Comparison Operations ===

    /**
     * @brief Compares this value with another using given operator.
     * @param rhs Right-hand operand
     * @param op Comparison operation to perform
     * @return Result of comparison or false if not supported
     */
    virtual bool compare(const LiteralValue& rhs, ComparisonOp op) const {
        return false;
    }

    // === Logical Operations ===

    /**
     * @brief Applies logical operation with another value.
     * @param rhs Right-hand operand
     * @param op Logical operation to perform
     * @return Result of operation or nullptr if not supported
     */
    virtual std::unique_ptr<LiteralValue> applyLogical(
        const LiteralValue& rhs, LogicalOp op) const {
        return nullptr;
    }

    // === Bitwise Operations ===

    /**
     * @brief Applies bitwise operation with another value.
     * @param rhs Right-hand operand
     * @param op Bitwise operation to perform
     * @return Result of operation or nullptr if not supported
     */
    virtual std::unique_ptr<LiteralValue> applyBitwise(
        const LiteralValue& rhs, BitwiseOp op) const {
        return nullptr;
    }

    // === String Operations ===

    /**
     * @brief Applies concatenation with another value.
     * @param rhs Right-hand operand
     * @param op Concatenation operation to perform
     * @return Result of concatenation or nullptr if not supported
     */
    virtual std::unique_ptr<LiteralValue> applyConcat(
        const LiteralValue& rhs, ConcatOp op = ConcatOp::CONCAT) const {
        return nullptr;
    }

    // === Pattern Matching ===

    /**
     * @brief Applies regex pattern matching with another value.
     * @param pattern Pattern to match against
     * @param op Regex operation to perform
     * @return Result of pattern matching or false if not supported
     */
    virtual bool applyRegex(
        const LiteralValue& pattern, RegexOp op) const {
        return false;
    }

    // === JSON Operations ===

    /**
     * @brief Applies JSON operation with another value.
     * @param rhs Right-hand operand
     * @param op JSON operation to perform
     * @return Result of operation or nullptr if not supported
     */
    virtual std::unique_ptr<LiteralValue> applyJson(
        const LiteralValue& rhs, JsonOp op) const {
        return nullptr;
    }

protected:
    /**
     * @brief Checks if given value is of specified type.
     * @tparam T Expected value type
     * @param value Value to check
     * @return true if value is of type T
     */
    template<typename T>
    bool isType(const LiteralValue& value) const {
        return dynamic_cast<const T*>(&value) != nullptr;
    }
};
