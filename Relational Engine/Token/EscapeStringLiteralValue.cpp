/**
 * @file EscapeStringLiteralValue.cpp
 * @brief Implementation of EscapeStringLiteralValue methods.
 * @details
 * Implements construction, unescaping, cloning, comparison,
 * validation, and raw value access for EscapeStringLiteralValue.
 */

#include "EscapeStringLiteralValue.hpp"
#include <sstream>
#include <stdexcept>

 // === Constructor ===

EscapeStringLiteralValue::EscapeStringLiteralValue(const std::string& v)
    : value(v)
{
    validate();
}

// === Core Interface ===

std::string EscapeStringLiteralValue::toString() const {
    return unescape(value);
}

std::unique_ptr<LiteralValue> EscapeStringLiteralValue::clone() const {
    return std::make_unique<EscapeStringLiteralValue>(value);
}

// === Validation ===

bool EscapeStringLiteralValue::validateEscapes(const std::string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\') {
            if (i + 1 >= s.size()) {
                return false; // Escape at end of string
            }
            switch (s[i + 1]) {
            case 'n': case 't': case 'r': case '\\':
            case '\'': case '"': case '0':
                ++i; // Skip next char
                break;
            default:
                return false; // Invalid escape sequence
            }
        }
    }
    return true;
}

bool EscapeStringLiteralValue::isValid() const {
    return validateEscapes(value);
}

void EscapeStringLiteralValue::validate() const {
    if (!isValid()) {
        throw std::invalid_argument("Invalid escape sequence in string");
    }
}

bool EscapeStringLiteralValue::equals(const LiteralValue& other) const {
    const auto* escOther = dynamic_cast<const EscapeStringLiteralValue*>(&other);
    return escOther && unescape(value) == unescape(escOther->value);
}

std::string EscapeStringLiteralValue::unescape(const std::string& s) {
    std::ostringstream out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            switch (s[i + 1]) {
            case 'n':  out << '\n'; ++i; break;
            case 't':  out << '\t'; ++i; break;
            case 'r':  out << '\r'; ++i; break;
            case '\\': out << '\\'; ++i; break;
            case '\'': out << '\''; ++i; break;
            case '"':  out << '"';  ++i; break;
            case '0':  out << '\0'; ++i; break;
            default:   out << s[i + 1]; ++i; break;
            }
        }
        else {
            out << s[i];
        }
    }
    return out.str();
}

// === Operations ===

bool EscapeStringLiteralValue::compare(const LiteralValue& rhs, ComparisonOp op) const {
    const auto* r = dynamic_cast<const EscapeStringLiteralValue*>(&rhs);
    if (!r) {
        return false;
    }

    std::string a = unescape(value);
    std::string b = unescape(r->value);

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

std::string EscapeStringLiteralValue::raw() const {
    return value;
}