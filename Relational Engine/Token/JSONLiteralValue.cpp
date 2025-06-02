/**
 * @file JSONLiteralValue.cpp
 * @brief Implementation of JSONLiteralValue methods.
 */

#include "JSONLiteralValue.hpp"
#include "StringLiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include "FloatLiteralValue.hpp"
#include "BooleanLiteralValue.hpp"
#include <stdexcept>

using json = nlohmann::json;

// === Constructors ===

JSONLiteralValue::JSONLiteralValue()
    : value("{}")
{
}

JSONLiteralValue::JSONLiteralValue(const std::string& v)
    : value(v)
{
    validate();
}

// === Core Interface ===

std::string JSONLiteralValue::toString() const {
    return value;
}

std::unique_ptr<LiteralValue> JSONLiteralValue::clone() const {
    return std::make_unique<JSONLiteralValue>(value);
}

// === Protected Methods ===

std::optional<json> JSONLiteralValue::parseJSON() const noexcept {
    try {
        return json::parse(value);
    }
    catch (...) {
        return std::nullopt;
    }
}

// === Validation ===

bool JSONLiteralValue::isValid() const {
    return parseJSON().has_value();
}

void JSONLiteralValue::validate() const {
    if (!parseJSON()) {
        throw std::invalid_argument("Invalid JSON format: " + value);
    }
}

bool JSONLiteralValue::equals(const LiteralValue& other) const {
    const auto* jsonOther =
        dynamic_cast<const JSONLiteralValue*>(&other);
    if (!jsonOther) return false;

    auto json1 = parseJSON();
    auto json2 = jsonOther->parseJSON();

    return json1.has_value() && json2.has_value() &&
        json1.value() == json2.value();
}

// === Operations ===

std::unique_ptr<LiteralValue> JSONLiteralValue::processJsonOp(
    JsonOp op,
    const json& j,
    const std::string& path) const
{
    try {
        if (op == JsonOp::ARROW || op == JsonOp::ARROW2) {
            std::string key = path;
            // Strip quotes if present
            if (!key.empty() && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            if (!j.contains(key)) return nullptr;

            const auto& sub = j[key];
            if (op == JsonOp::ARROW) {
                return std::make_unique<JSONLiteralValue>(sub.dump());
            }
            else { // ARROW2
                return sub.is_string()
                    ? std::make_unique<StringLiteralValue>(sub.get<std::string>())
                    : std::make_unique<StringLiteralValue>(sub.dump());
            }
        }
        else if (op == JsonOp::HASH_ARROW || op == JsonOp::HASH_ARROW2) {
            json pathArray = json::parse(path);
            if (!pathArray.is_array()) return nullptr;

            const json* ptr = &j;
            for (const auto& k : pathArray) {
                if (!k.is_string()) return nullptr;
                const std::string& step = k.get_ref<const std::string&>();
                if (!ptr->contains(step)) return nullptr;
                ptr = &(*ptr)[step];
            }

            if (op == JsonOp::HASH_ARROW) {
                return std::make_unique<JSONLiteralValue>(ptr->dump());
            }
            else { // HASH_ARROW2
                return ptr->is_string()
                    ? std::make_unique<StringLiteralValue>(ptr->get<std::string>())
                    : std::make_unique<StringLiteralValue>(ptr->dump());
            }
        }
    }
    catch (...) {
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<LiteralValue> JSONLiteralValue::applyJson(
    const LiteralValue& rhs, JsonOp op) const
{
    auto j = parseJSON();
    if (!j) return nullptr;

    const auto* r = dynamic_cast<const JSONLiteralValue*>(&rhs);
    if (!r) return nullptr;

    return processJsonOp(op, j.value(), r->value);
}

bool JSONLiteralValue::compare(
    const LiteralValue& rhs, ComparisonOp op) const
{
    const auto* r = dynamic_cast<const JSONLiteralValue*>(&rhs);
    if (!r) return false;

    auto json1 = parseJSON();
    auto json2 = r->parseJSON();

    if (!json1 || !json2) return false;

    const auto& a = json1.value();
    const auto& b = json2.value();

    switch (op) {
    case ComparisonOp::EQUAL:         return a == b;
    case ComparisonOp::NOT_EQUAL:     return a != b;
    case ComparisonOp::LESS:          return a < b;
    case ComparisonOp::GREATER:       return a > b;
    case ComparisonOp::LESS_EQUAL:    return a <= b;
    case ComparisonOp::GREATER_EQUAL: return a >= b;
    default:                          return false;
    }
}

// === Type Checks ===

bool JSONLiteralValue::isObject() const noexcept {
    auto j = parseJSON();
    return j && j->is_object();
}

bool JSONLiteralValue::isArray() const noexcept {
    auto j = parseJSON();
    return j && j->is_array();
}

bool JSONLiteralValue::isString() const noexcept {
    auto j = parseJSON();
    return j && j->is_string();
}

bool JSONLiteralValue::isNumber() const noexcept {
    auto j = parseJSON();
    return j && j->is_number();
}

bool JSONLiteralValue::isBoolean() const noexcept {
    auto j = parseJSON();
    return j && j->is_boolean();
}

bool JSONLiteralValue::isNull() const noexcept {
    auto j = parseJSON();
    return j && j->is_null();
}

// === Access Operations ===

bool JSONLiteralValue::contains(const std::string& key) const noexcept {
    auto j = parseJSON();
    return j && j->contains(key);
}

std::unique_ptr<LiteralValue>
JSONLiteralValue::getKey(const std::string& key) const noexcept {
    auto j = parseJSON();
    if (!j || !j->contains(key)) return nullptr;
    return std::make_unique<JSONLiteralValue>((*j)[key].dump());
}

std::unique_ptr<LiteralValue>
JSONLiteralValue::getPath(const std::vector<std::string>& path) const noexcept {
    auto j = parseJSON();
    if (!j) return nullptr;

    const json* ptr = &j.value();
    for (const auto& k : path) {
        if (!ptr->contains(k)) return nullptr;
        ptr = &(*ptr)[k];
    }
    return std::make_unique<JSONLiteralValue>(ptr->dump());
}

// === Type Conversions ===

std::unique_ptr<LiteralValue>
JSONLiteralValue::toStringLiteral() const noexcept {
    auto j = parseJSON();
    if (!j) return nullptr;

    return j->is_string()
        ? std::make_unique<StringLiteralValue>(j->get<std::string>())
        : std::make_unique<StringLiteralValue>(j->dump());
}

std::unique_ptr<LiteralValue>
JSONLiteralValue::toIntegerLiteral() const noexcept {
    auto j = parseJSON();
    if (!j) return nullptr;

    if (j->is_number_integer()) {
        return std::make_unique<IntegerLiteralValue>(j->get<int64_t>());
    }
    if (j->is_number()) {
        return std::make_unique<IntegerLiteralValue>(
            static_cast<int64_t>(j->get<double>()));
    }
    return nullptr;
}

std::unique_ptr<LiteralValue>
JSONLiteralValue::toFloatLiteral() const noexcept {
    auto j = parseJSON();
    if (!j || !j->is_number()) return nullptr;
    return std::make_unique<FloatLiteralValue>(j->get<double>());
}

std::unique_ptr<LiteralValue>
JSONLiteralValue::toBooleanLiteral() const noexcept {
    auto j = parseJSON();
    if (!j || !j->is_boolean()) return nullptr;
    return std::make_unique<BooleanLiteralValue>(j->get<bool>());
}