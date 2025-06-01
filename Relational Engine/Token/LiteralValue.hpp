#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <cstdint>
#include "TokenEnums.hpp"

/**
 * @file LiteralValue.hpp
 * @brief Base class and subclasses for all literal values used in tokens.
 * @details
 * Provides an abstract interface and concrete implementations for SQL literal values.
 * Used for typed value storage, comparison, and evaluation in SQL parsing and execution.
 */

 // -----------------------------------------------------------------------------
 // Base class for all literal values
 // -----------------------------------------------------------------------------

 /**
  * @class LiteralValue
  * @brief Abstract base class for all literal values.
  * @details
  * Provides virtual interfaces for string conversion, cloning, and value operations.
  */
class LiteralValue {
public:
    virtual ~LiteralValue() = default;

    /**
     * @brief Converts the literal value to a string representation.
     * @return String value.
     */
    virtual std::string toString() const = 0;

    /**
     * @brief Clones the literal value.
     * @return Unique pointer to a copy of the value.
     */
    virtual std::unique_ptr<LiteralValue> clone() const = 0;

    // Arithmetic operations
    virtual std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const;
    // Assignment operations
    virtual std::unique_ptr<LiteralValue> applyAssign(const LiteralValue& rhs, AssignOp op) const;
    // Comparison operations
    virtual bool compare(const LiteralValue& rhs, ComparisonOp op) const;
    // Logical operations
    virtual std::unique_ptr<LiteralValue> applyLogical(const LiteralValue& rhs, LogicalOp op) const;
    // Bitwise operations
    virtual std::unique_ptr<LiteralValue> applyBitwise(const LiteralValue& rhs, BitwiseOp op) const;
    // Concatenation (for strings, arrays, etc.)
    virtual std::unique_ptr<LiteralValue> applyConcat(const LiteralValue& rhs, ConcatOp op = ConcatOp::CONCAT) const;
    // JSON operations
    virtual std::unique_ptr<LiteralValue> applyJson(const LiteralValue& rhs, JsonOp op) const;
    // Regex operations (for pattern matching)
    virtual bool applyRegex(const LiteralValue& pattern, RegexOp op) const;
};

// -----------------------------------------------------------------------------
// Integer literal
// -----------------------------------------------------------------------------

/**
 * @class IntegerLiteralValue
 * @brief Represents an integer literal value.
 */
class IntegerLiteralValue : public LiteralValue {
public:
    int64_t value; ///< The integer value.

    explicit IntegerLiteralValue(int64_t v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const override;
    std::unique_ptr<LiteralValue> applyBitwise(const LiteralValue& rhs, BitwiseOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// Floating-point literal
// -----------------------------------------------------------------------------

/**
 * @class FloatLiteralValue
 * @brief Represents a floating-point literal value.
 */
class FloatLiteralValue : public LiteralValue {
public:
    double value; ///< The floating-point value.

    explicit FloatLiteralValue(double v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// String literal
// -----------------------------------------------------------------------------

/**
 * @class StringLiteralValue
 * @brief Represents a string literal value.
 */
class StringLiteralValue : public LiteralValue {
public:
    std::string value; ///< The string value.

    explicit StringLiteralValue(std::string v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    std::unique_ptr<LiteralValue> applyConcat(const LiteralValue& rhs, ConcatOp op = ConcatOp::CONCAT) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
    bool applyRegex(const LiteralValue& pattern, RegexOp op) const override;
};

// -----------------------------------------------------------------------------
// Char literal (single Unicode code unit, usually for SQL CHAR or C-style char)
// -----------------------------------------------------------------------------

/**
 * @class CharLiteralValue
 * @brief Represents a character literal value (single Unicode code unit).
 */
class CharLiteralValue : public LiteralValue {
public:
    char value; ///< The character value.

    explicit CharLiteralValue(char v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// Escape String literal (for storing strings with escape sequences)
// -----------------------------------------------------------------------------

/**
 * @class EscapeStringLiteralValue
 * @brief Stores a string literal with escape sequences.
 * @details
 * Provides access to the raw (escaped) string and unescaped value.
 */
class EscapeStringLiteralValue : public LiteralValue {
public:
    std::string value; ///< Stores the escaped string.

    explicit EscapeStringLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    /**
     * @brief Returns the raw (escaped) string.
     */
    std::string raw() const;
};

// -----------------------------------------------------------------------------
// Boolean literal
// -----------------------------------------------------------------------------

/**
 * @class BooleanLiteralValue
 * @brief Represents a boolean literal value.
 */
class BooleanLiteralValue : public LiteralValue {
public:
    bool value; ///< The boolean value.

    explicit BooleanLiteralValue(bool v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    std::unique_ptr<LiteralValue> applyLogical(const LiteralValue& rhs, LogicalOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// Null literal
// -----------------------------------------------------------------------------

/**
 * @class NullLiteralValue
 * @brief Represents a SQL NULL literal value.
 */
class NullLiteralValue : public LiteralValue {
public:
    NullLiteralValue();

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// Binary/hex literal
// -----------------------------------------------------------------------------

/**
 * @class BinaryLiteralValue
 * @brief Represents a binary or hexadecimal literal value.
 */
class BinaryLiteralValue : public LiteralValue {
public:
    std::vector<uint8_t> value; ///< The binary value.

    explicit BinaryLiteralValue(const std::vector<uint8_t>& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};

// -----------------------------------------------------------------------------
// Date literal
// -----------------------------------------------------------------------------

/**
 * @class DateLiteralValue
 * @brief Represents a date literal value (YYYY-MM-DD).
 */
class DateLiteralValue : public LiteralValue {
public:
    std::string value; ///< Date string in ISO format.

    explicit DateLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
    std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const override;
};

// -----------------------------------------------------------------------------
// Time literal
// -----------------------------------------------------------------------------

/**
 * @class TimeLiteralValue
 * @brief Represents a time literal value (HH:MM:SS).
 */
class TimeLiteralValue : public LiteralValue {
public:
    std::string value; ///< Time string in ISO format.

    explicit TimeLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
    std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const override;
};

// -----------------------------------------------------------------------------
// DateTime literal
// -----------------------------------------------------------------------------

/**
 * @class DateTimeLiteralValue
 * @brief Represents a date-time literal value (YYYY-MM-DD HH:MM:SS).
 */
class DateTimeLiteralValue : public LiteralValue {
public:
    std::string value; ///< DateTime string in ISO format.

    explicit DateTimeLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
    std::unique_ptr<LiteralValue> applyArithmetic(const LiteralValue& rhs, ArithmeticOp op) const override;
};

// -----------------------------------------------------------------------------
// UUID literal (as string, with real UUID operations)
// -----------------------------------------------------------------------------

/**
 * @class UUIDLiteralValue
 * @brief Represents a UUID literal value (canonical UUID string).
 */
class UUIDLiteralValue : public LiteralValue {
public:
    std::string value; ///< Canonical UUID string.

    explicit UUIDLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // Check UUID validity
    bool isValid() const;

    // Generate new random UUID (static)
    static UUIDLiteralValue generate();
};

// -----------------------------------------------------------------------------
// JSON literal (as string, with rich JSON operations)
// -----------------------------------------------------------------------------

/**
 * @class JSONLiteralValue
 * @brief Represents a JSON literal value (as string).
 * @details
 * Provides SQL-like JSON operations and rich type/content checks.
 */
class JSONLiteralValue : public LiteralValue {
public:
    std::string value; ///< JSON value as string.

    explicit JSONLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;
    std::unique_ptr<LiteralValue> applyJson(const LiteralValue& rhs, JsonOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // --- Extra methods for DB core (all noexcept if parse fails) ---

    // Check JSON type
    bool isObject() const;    ///< True if JSON is an object.
    bool isArray() const;     ///< True if JSON is an array.
    bool isString() const;    ///< True if JSON is a string.
    bool isNumber() const;    ///< True if JSON is a number.
    bool isBoolean() const;   ///< True if JSON is a boolean.
    bool isNull() const;      ///< True if JSON is null.

    // Check if key exists at top level (object only)
    bool contains(const std::string& key) const;

    // Get value by key (object only), result is JSON
    std::unique_ptr<LiteralValue> getKey(const std::string& key) const;

    // Get value by path (vector of keys), result is JSON
    std::unique_ptr<LiteralValue> getPath(const std::vector<std::string>& path) const;

    // Cast to string/number/bool (for SQL semantics)
    std::unique_ptr<LiteralValue> toStringLiteral() const;
    std::unique_ptr<LiteralValue> toIntegerLiteral() const;
    std::unique_ptr<LiteralValue> toFloatLiteral() const;
    std::unique_ptr<LiteralValue> toBooleanLiteral() const;
};

// -----------------------------------------------------------------------------
// XML literal (as string, with real XML operations)
// -----------------------------------------------------------------------------

/**
 * @class XMLLiteralValue
 * @brief Represents an XML literal value (as string).
 * @details
 * Provides XPath and element/attribute access.
 */
class XMLLiteralValue : public LiteralValue {
public:
    std::string value; ///< XML value as string.

    explicit XMLLiteralValue(const std::string& v);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // XML operations (XPath, element/attribute access)
    std::unique_ptr<LiteralValue> applyXml(const std::string& xpath) const;

    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // Check if XML is valid and root node exists
    bool isValid() const;
    bool hasElement(const std::string& name) const;

    // Get attribute value by name (from root)
    std::unique_ptr<LiteralValue> getAttribute(const std::string& attr) const;

    // Get text content of root element
    std::unique_ptr<LiteralValue> getText() const;

    // Find elements by XPath (returns XML or string literal)
    std::vector<std::unique_ptr<LiteralValue>> findByXPath(const std::string& xpath) const;
};

// -----------------------------------------------------------------------------
// Array literal (vector of LiteralValue)
// -----------------------------------------------------------------------------

/**
 * @class ArrayLiteralValue
 * @brief Represents an array literal value (vector of LiteralValue).
 */
class ArrayLiteralValue : public LiteralValue {
public:
    std::vector<std::shared_ptr<LiteralValue>> elements; ///< Elements of the array.

    explicit ArrayLiteralValue(std::vector<std::shared_ptr<LiteralValue>> elems);

    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    std::unique_ptr<LiteralValue> applyConcat(const LiteralValue& rhs, ConcatOp op = ConcatOp::CONCAT) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;
};