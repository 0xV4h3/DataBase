#pragma once

#include <string>
#include <memory>
#include <utility>
#include "TokenEnums.hpp"
#include "KeywordInfo.hpp"
#include "FunctionInfo.hpp"
#include "IdentifierInfo.hpp"
#include "LiteralValue.hpp"
#include "OperatorInfo.hpp"
#include "PunctuatorInfo.hpp"

// ---------------- Base Token class and derived token classes ----------------

typedef TokenType TT;

/**
 * @class Token
 * @brief Base class for all tokens in the SQL parser.
 * @details
 * Represents a single token extracted during SQL parsing.
 * Stores token type, value (lexeme), and position in the source string.
 *
 * @see KeywordToken
 * @see FunctionToken
 * @see IdentifierToken
 * @see LiteralToken
 * @see OperatorToken
 * @see PunctuatorToken
 * @see DateTimePartToken
 * @see DollarQuoteToken
 * @see CommentToken
 */
class Token {
public:
    Token() : type_(TT::UNKNOWN), value_(), position_(-1) {}
    Token(TT t, std::string v, int p) : type_(t), value_(std::move(v)), position_(p) {}
    Token(TT t, std::string v) : type_(t), value_(std::move(v)), position_(-1) {}
    virtual ~Token() = default;

    /**
     * @brief Returns the token type.
     */
    TT getType() const { return type_; }

    /**
     * @brief Returns the string value (lexeme) of the token.
     */
    const std::string& getValue() const { return value_; }

    /**
     * @brief Returns the position of the token in the source (or -1 if unknown).
     */
    int getPosition() const { return position_; }

    /**
     * @brief Sets the position of the token in the source.
     * @param position The position (use -1 for unknown).
     */
    void setPosition(int position) { position_ = (position >= 0 ? position : -1); }

protected:
    TT type_;             ///< Token type (enum value)
    std::string value_;   ///< String value (lexeme) of the token
    int position_;        ///< Position in the source string
};

/**
 * @class KeywordToken
 * @brief Token representing a SQL keyword.
 * @details
 * Stores a reference to KeywordInfo, which contains metadata about the keyword (such as lexeme, category, etc).
 * @see Token
 * @see KeywordInfo
 */
class KeywordToken : public Token {
public:
    std::shared_ptr<KeywordInfo> infoPtr; ///< Pointer to keyword metadata

    /**
     * @brief Constructs a KeywordToken with the given KeywordInfo.
     * @param info Shared pointer to KeywordInfo.
     */
    KeywordToken(std::shared_ptr<KeywordInfo> info)
        : Token(TT::KEYWORD, info ? info->lexeme : ""), infoPtr(std::move(info)) {
    }

    /**
     * @brief Constructs an empty KeywordToken (invalid/unknown).
     */
    KeywordToken() : Token(TT::KEYWORD, "", -1), infoPtr(nullptr) {}
};

/**
 * @class FunctionToken
 * @brief Token representing a SQL function.
 * @details
 * Stores a reference to FunctionInfo, which describes the function and its metadata.
 * @see Token
 * @see FunctionInfo
 */
class FunctionToken : public Token {
public:
    std::shared_ptr<FunctionInfo> infoPtr; ///< Pointer to function metadata

    /**
     * @brief Constructs a FunctionToken with the given FunctionInfo.
     * @param info Shared pointer to FunctionInfo.
     */
    FunctionToken(std::shared_ptr<FunctionInfo> info)
        : Token(TT::FUNCTION, info ? info->name : ""), infoPtr(std::move(info)) {
    }

    /**
     * @brief Constructs an empty FunctionToken (invalid/unknown).
     */
    FunctionToken() : Token(TT::FUNCTION, "", -1), infoPtr(nullptr) {}
};

/**
 * @class IdentifierToken
 * @brief Token representing a SQL identifier (table, column, alias, etc).
 * @details
 * Stores the identifier string and a pointer to IdentifierInfo with semantic information.
 * @see Token
 * @see IdentifierInfo
 */
class IdentifierToken : public Token {
public:
    std::shared_ptr<IdentifierInfo> infoPtr; ///< Pointer to identifier metadata

    /**
     * @brief Constructs an IdentifierToken with the given string and IdentifierInfo.
     * @param v The identifier string.
     * @param info Shared pointer to IdentifierInfo.
     */
    IdentifierToken(std::string v, std::shared_ptr<IdentifierInfo> info)
        : Token(TT::IDENTIFIER, std::move(v)), infoPtr(std::move(info)) {
    }

    /**
     * @brief Constructs an empty IdentifierToken (invalid/unknown).
     */
    IdentifierToken() : Token(TT::IDENTIFIER, "", -1), infoPtr(nullptr) {}
};

/**
 * @class LiteralToken
 * @brief Token representing a SQL literal value.
 * @details
 * Stores the literal's category and a pointer to its value object.
 * @see Token
 * @see LiteralValue
 */
class LiteralToken : public Token {
public:
    LiteralCategory litType;                      ///< Category of the literal
    std::shared_ptr<LiteralValue> valuePtr;       ///< Pointer to the literal value object

    /**
     * @brief Constructs a LiteralToken with the given category and string value.
     * @param lt The literal category.
     * @param v The string value.
     */
    LiteralToken(LiteralCategory lt, std::string v)
        : Token(TT::LITERAL, std::move(v)), litType(lt), valuePtr(nullptr) {
    }

    /**
     * @brief Constructs a LiteralToken with category, string value, and value object.
     * @param lt The literal category.
     * @param v The string value.
     * @param val Shared pointer to the value object.
     */
    LiteralToken(LiteralCategory lt, std::string v, std::shared_ptr<LiteralValue> val)
        : Token(TT::LITERAL, std::move(v)), litType(lt), valuePtr(std::move(val)) {
    }

    /**
     * @brief Constructs an empty LiteralToken (invalid/unknown).
     */
    LiteralToken() : Token(TT::LITERAL, "", -1), litType(LiteralCategory::UNKNOWN), valuePtr(nullptr) {}
};

/**
 * @class OperatorToken
 * @brief Token representing a SQL operator.
 * @details
 * Stores a reference to OperatorInfo with operator metadata.
 * @see Token
 * @see OperatorInfo
 */
class OperatorToken : public Token {
public:
    std::shared_ptr<OperatorInfo> infoPtr; ///< Pointer to operator metadata

    /**
     * @brief Constructs an OperatorToken with the given OperatorInfo.
     * @param info Shared pointer to OperatorInfo.
     */
    OperatorToken(std::shared_ptr<OperatorInfo> info)
        : Token(TT::OPERATOR, info ? info->symbol : ""), infoPtr(std::move(info)) {
    }

    /**
     * @brief Constructs an empty OperatorToken (invalid/unknown).
     */
    OperatorToken() : Token(TT::OPERATOR, "", -1), infoPtr(nullptr) {}
};

/**
 * @class DateTimePartToken
 * @brief Token representing a SQL date/time part (e.g. YEAR, MONTH).
 * @details
 * Used for tokens that refer to specific parts of date/time values.
 * @see Token
 * @see DateTimePart
 */
class DateTimePartToken : public Token {
public:
    DateTimePart part; ///< The datetime part represented by this token

    /**
     * @brief Constructs a DateTimePartToken.
     * @param p The date/time part (enum).
     * @param v The string representation.
     */
    DateTimePartToken(DateTimePart p, std::string v)
        : Token(TT::DATETIMEPART, std::move(v)), part(p) {
    }

    /**
     * @brief Returns the DateTimePart enum value.
     */
    DateTimePart getPart() const { return part; }
};

/**
 * @class PunctuatorToken
 * @brief Token representing a SQL punctuator (symbol, delimiter).
 * @details
 * Stores a reference to PunctuatorInfo for metadata.
 * @see Token
 * @see PunctuatorInfo
 */
class PunctuatorToken : public Token {
public:
    std::shared_ptr<PunctuatorInfo> infoPtr; ///< Pointer to punctuator metadata

    /**
     * @brief Constructs a PunctuatorToken with the given PunctuatorInfo.
     * @param info Shared pointer to PunctuatorInfo.
     */
    PunctuatorToken(std::shared_ptr<PunctuatorInfo> info)
        : Token(TT::PUNCTUATOR, info ? info->lexeme : ""), infoPtr(std::move(info)) {
    }

    /**
     * @brief Constructs an empty PunctuatorToken (invalid/unknown).
     */
    PunctuatorToken() : Token(TT::PUNCTUATOR, "", -1), infoPtr(nullptr) {}
};

/**
 * @class DollarQuoteToken
 * @brief Token representing a dollar-quoted string.
 * @details
 * Used for PostgreSQL-style dollar-quoted string literals.
 * @see Token
 */
class DollarQuoteToken : public Token {
public:
    /**
     * @brief Constructs a DollarQuoteToken with the given string value.
     * @param v The quoted string value.
     */
    DollarQuoteToken(std::string v) : Token(TT::LITERAL, std::move(v)) {}
};

/**
 * @class CommentToken
 * @brief Token representing a SQL comment.
 * @details
 * Stores the comment type (single-line, multi-line, etc).
 * @see Token
 * @see CommentType
 */
class CommentToken : public Token {
public:
    CommentType commentType; ///< Type of comment (enum)

    /**
     * @brief Constructs a CommentToken with the given type and value.
     * @param t The comment type (enum).
     * @param v The comment string.
     */
    CommentToken(CommentType t, std::string v)
        : Token(TT::COMMENT, std::move(v)), commentType(t) {
    }
};