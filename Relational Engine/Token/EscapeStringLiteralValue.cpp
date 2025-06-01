/**
 * @file EscapeStringLiteralValue.cpp
 * @brief Implementation of EscapeStringLiteralValue methods.
 * @details
 * Implements construction, unescaping, cloning, comparison, and raw value access for EscapeStringLiteralValue.
 */

 // --- Constructor ---
 // --- Return unescaped string ---
 // --- Deep copy ---
 // --- Compare by unescaped value ---
 // --- Return raw (escaped) string ---

#include "LiteralValue.hpp"
#include <memory>
#include <sstream>

// --- Helper for unescaping ---
static std::string unescape(const std::string& s) {
    std::ostringstream out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            switch (s[i + 1]) {
            case 'n': out << '\n'; ++i; break;
            case 't': out << '\t'; ++i; break;
            case 'r': out << '\r'; ++i; break;
            case '\\': out << '\\'; ++i; break;
            case '\'': out << '\''; ++i; break;
            case '\"': out << '\"'; ++i; break;
            case '0': out << '\0'; ++i; break;
            default: out << s[i + 1]; ++i; break;
            }
        }
        else {
            out << s[i];
        }
    }
    return out.str();
}

// --- Constructor ---
EscapeStringLiteralValue::EscapeStringLiteralValue(const std::string& v) : value(v) {}

// --- Return unescaped string ---
std::string EscapeStringLiteralValue::toString() const {
    return unescape(value);
}

// --- Deep copy ---
std::unique_ptr<LiteralValue> EscapeStringLiteralValue::clone() const {
    return std::make_unique<EscapeStringLiteralValue>(value);
}

// --- Compare by unescaped value ---
bool EscapeStringLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const EscapeStringLiteralValue*>(&rhs);
    if (!r) return false;
    std::string a = unescape(value);
    std::string b = unescape(r->value);
    switch (op) {
    case ComparisonOp::EQUAL:        return a == b;
    case ComparisonOp::NOT_EQUAL:    return a != b;
    case ComparisonOp::LESS:         return a < b;
    case ComparisonOp::GREATER:      return b < a;
    case ComparisonOp::LESS_EQUAL:   return !(b < a);
    case ComparisonOp::GREATER_EQUAL:return !(a < b);
    default: return false;
    }
}

// --- Return raw (escaped) string ---
std::string EscapeStringLiteralValue::raw() const {
    return value;
}