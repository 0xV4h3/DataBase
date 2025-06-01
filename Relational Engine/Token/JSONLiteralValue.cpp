/**
 * @file JSONLiteralValue.cpp
 * @brief Implementation of JSONLiteralValue methods.
 * @details
 * Implements construction, string conversion, deep copy, SQL-like JSON operations, comparison,
 * type checks, key/path retrieval, and cast operations for JSONLiteralValue.
 */

#include "LiteralValue.hpp"
#include <json.hpp>
#include <stdexcept>
#include <sstream>

using json = nlohmann::json;

// --- Constructor ---
JSONLiteralValue::JSONLiteralValue(const std::string& v) : value(v) {}

// --- String serialization (returns as is) ---
std::string JSONLiteralValue::toString() const {
    return value;
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> JSONLiteralValue::clone() const {
    return std::make_unique<JSONLiteralValue>(value);
}

// --- SQL-like JSON operators: ->, ->>, #>, #>> ---
//  - ARROW/ARROW2: rhs.value is key (string)
//  - HASH_ARROW/HASH_ARROW2: rhs.value is JSON array of keys (as string)
std::unique_ptr<LiteralValue> JSONLiteralValue::applyJson(const LiteralValue& rhs, JsonOp op) const {
    try {
        json j = json::parse(value);
        if (const auto* r = dynamic_cast<const JSONLiteralValue*>(&rhs)) {
            if (op == JsonOp::ARROW || op == JsonOp::ARROW2) {
                std::string key = r->value;
                // Strip quotes if present
                if (!key.empty() && key.front() == '"' && key.back() == '"')
                    key = key.substr(1, key.size() - 2);
                if (!j.contains(key)) return nullptr;
                const auto& sub = j[key];
                if (op == JsonOp::ARROW)
                    return std::make_unique<JSONLiteralValue>(sub.dump());
                else // ARROW2
                    return sub.is_string()
                    ? std::make_unique<StringLiteralValue>(sub.get<std::string>())
                    : std::make_unique<StringLiteralValue>(sub.dump());
            }
            if (op == JsonOp::HASH_ARROW || op == JsonOp::HASH_ARROW2) {
                json path = json::parse(r->value);
                if (!path.is_array()) return nullptr;
                json* ptr = &j;
                for (const auto& k : path) {
                    if (!k.is_string()) return nullptr;
                    const std::string& step = k.get_ref<const std::string&>();
                    if (!ptr->contains(step)) return nullptr;
                    ptr = &(*ptr)[step];
                }
                if (op == JsonOp::HASH_ARROW)
                    return std::make_unique<JSONLiteralValue>(ptr->dump());
                else // HASH_ARROW2
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

// --- Compare by parsed JSON, not by text ---
bool JSONLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    try {
        const auto* r = dynamic_cast<const JSONLiteralValue*>(&rhs);
        if (!r) return false;
        json a = json::parse(value);
        json b = json::parse(r->value);
        switch (op) {
        case ComparisonOp::EQUAL:        return a == b;
        case ComparisonOp::NOT_EQUAL:    return a != b;
        case ComparisonOp::LESS:         return a < b;
        case ComparisonOp::GREATER:      return a > b;
        case ComparisonOp::LESS_EQUAL:   return a <= b;
        case ComparisonOp::GREATER_EQUAL:return a >= b;
        default: return false;
        }
    }
    catch (...) {
        return false;
    }
}

// --- JSON type checks (object/array/string/number/bool/null) ---
bool JSONLiteralValue::isObject() const {
    try { return json::parse(value).is_object(); }
    catch (...) { return false; }
}
bool JSONLiteralValue::isArray() const {
    try { return json::parse(value).is_array(); }
    catch (...) { return false; }
}
bool JSONLiteralValue::isString() const {
    try { return json::parse(value).is_string(); }
    catch (...) { return false; }
}
bool JSONLiteralValue::isNumber() const {
    try { return json::parse(value).is_number(); }
    catch (...) { return false; }
}
bool JSONLiteralValue::isBoolean() const {
    try { return json::parse(value).is_boolean(); }
    catch (...) { return false; }
}
bool JSONLiteralValue::isNull() const {
    try { return json::parse(value).is_null(); }
    catch (...) { return false; }
}

// --- Check if key exists at top level ---
bool JSONLiteralValue::contains(const std::string& key) const {
    try {
        json j = json::parse(value);
        return j.contains(key);
    }
    catch (...) { return false; }
}

// --- Get value by key (object only), result is JSON ---
std::unique_ptr<LiteralValue> JSONLiteralValue::getKey(const std::string& key) const {
    try {
        json j = json::parse(value);
        if (!j.contains(key)) return nullptr;
        return std::make_unique<JSONLiteralValue>(j[key].dump());
    }
    catch (...) { return nullptr; }
}

// --- Get value by path (vector of keys), result is JSON ---
std::unique_ptr<LiteralValue> JSONLiteralValue::getPath(const std::vector<std::string>& path) const {
    try {
        json j = json::parse(value);
        json* ptr = &j;
        for (const auto& k : path) {
            if (!ptr->contains(k)) return nullptr;
            ptr = &(*ptr)[k];
        }
        return std::make_unique<JSONLiteralValue>(ptr->dump());
    }
    catch (...) { return nullptr; }
}

// --- Cast to string literal (if string, else dump entire JSON) ---
std::unique_ptr<LiteralValue> JSONLiteralValue::toStringLiteral() const {
    try {
        json j = json::parse(value);
        if (j.is_string())
            return std::make_unique<StringLiteralValue>(j.get<std::string>());
        return std::make_unique<StringLiteralValue>(j.dump());
    }
    catch (...) {
        return nullptr;
    }
}

// --- Cast to integer literal (if int or float, else nullptr) ---
std::unique_ptr<LiteralValue> JSONLiteralValue::toIntegerLiteral() const {
    try {
        json j = json::parse(value);
        if (j.is_number_integer())
            return std::make_unique<IntegerLiteralValue>(j.get<int64_t>());
        if (j.is_number())
            return std::make_unique<IntegerLiteralValue>(static_cast<int64_t>(j.get<double>()));
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}

// --- Cast to float literal (if number, else nullptr) ---
std::unique_ptr<LiteralValue> JSONLiteralValue::toFloatLiteral() const {
    try {
        json j = json::parse(value);
        if (j.is_number())
            return std::make_unique<FloatLiteralValue>(j.get<double>());
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}

// --- Cast to boolean literal (if bool, else nullptr) ---
std::unique_ptr<LiteralValue> JSONLiteralValue::toBooleanLiteral() const {
    try {
        json j = json::parse(value);
        if (j.is_boolean())
            return std::make_unique<BooleanLiteralValue>(j.get<bool>());
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}