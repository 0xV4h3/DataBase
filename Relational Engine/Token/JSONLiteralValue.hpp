/**
 * @file JSONLiteralValue.hpp
 * @brief Definition of JSONLiteralValue class.
 */

#pragma once
#include "LiteralValue.hpp"
#include <string>
#include <vector>
#include <json.hpp>

class JSONLiteralValue : public LiteralValue {
public:
    // === Constructors and Destructor ===
    explicit JSONLiteralValue(const std::string& v);
    JSONLiteralValue(); // Creates empty JSON object

    // Rule of five
    JSONLiteralValue(const JSONLiteralValue&) = default;
    JSONLiteralValue(JSONLiteralValue&&) noexcept = default;
    JSONLiteralValue& operator=(const JSONLiteralValue&) = default;
    JSONLiteralValue& operator=(JSONLiteralValue&&) noexcept = default;
    ~JSONLiteralValue() override = default;

    // === Core Interface ===
    std::string toString() const override;
    std::unique_ptr<LiteralValue> clone() const override;

    // === Validation ===
    bool isValid() const override;
    void validate() const override;
    bool equals(const LiteralValue& other) const override;

    // === Operations ===
    std::unique_ptr<LiteralValue> applyJson(
        const LiteralValue& rhs, JsonOp op) const override;
    bool compare(const LiteralValue& rhs, ComparisonOp op) const override;

    // === Type Checks ===
    bool isObject() const noexcept;
    bool isArray() const noexcept;
    bool isString() const noexcept;
    bool isNumber() const noexcept;
    bool isBoolean() const noexcept;
    bool isNull() const noexcept;

    // === Access Operations ===
    bool contains(const std::string& key) const noexcept;
    std::unique_ptr<LiteralValue> getKey(const std::string& key) const noexcept;
    std::unique_ptr<LiteralValue> getPath(
        const std::vector<std::string>& path) const noexcept;

    // === Type Conversions ===
    std::unique_ptr<LiteralValue> toStringLiteral() const noexcept;
    std::unique_ptr<LiteralValue> toIntegerLiteral() const noexcept;
    std::unique_ptr<LiteralValue> toFloatLiteral() const noexcept;
    std::unique_ptr<LiteralValue> toBooleanLiteral() const noexcept;

    // === Getters ===
    const std::string& getValue() const { return value; }

private:
    std::string value;  ///< JSON value as string

protected:
    /**
     * @brief Parses JSON string to nlohmann::json object.
     * @return Optional containing parsed JSON if valid
     */
    std::optional<nlohmann::json> parseJSON() const noexcept;

    /**
     * @brief Processes JSON path operations.
     * @param op Operation type (ARROW, ARROW2, HASH_ARROW, HASH_ARROW2)
     * @param json Base JSON object
     * @param path Path or key to process
     * @return Result as LiteralValue
     */
    std::unique_ptr<LiteralValue> processJsonOp(
        JsonOp op,
        const nlohmann::json& json,
        const std::string& path) const;
};