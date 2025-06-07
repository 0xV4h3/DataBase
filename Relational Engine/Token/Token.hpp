#pragma once

#include <string>
#include <memory>
#include <utility>
#include <sstream>
#include "TokenEnums.hpp"

// Include all Literal Value types
#include "LiteralValue.hpp"
#include "IntegerLiteralValue.hpp"
#include "FloatLiteralValue.hpp"
#include "StringLiteralValue.hpp"
#include "CharLiteralValue.hpp"
#include "BooleanLiteralValue.hpp"
#include "NullLiteralValue.hpp"
#include "BinaryLiteralValue.hpp"
#include "HEXLiteralValue.hpp"
#include "ArrayLiteralValue.hpp"
#include "DateLiteralValue.hpp"
#include "TimeLiteralValue.hpp"
#include "DateTimeLiteralValue.hpp"
#include "IntervalLiteralValue.hpp"
#include "UUIDLiteralValue.hpp"
#include "JSONLiteralValue.hpp"
#include "XMLLiteralValue.hpp"
#include "EscapeStringLiteralValue.hpp"

// Include other token info classes
#include "KeywordInfo.hpp"
#include "FunctionInfo.hpp"
#include "IdentifierInfo.hpp"
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
 * Provides common functionality for token manipulation and comparison.
 *
 * @see KeywordToken
 * @see FunctionToken
 * @see IdentifierToken
 * @see LiteralToken
 * @see OperatorToken
 * @see DateTimePartToken
 * @see CommentToken
 */
class Token {
public:
    // === Constructors and Destructor ===

    /**
     * @brief Default constructor.
     * @details Creates an unknown token with no value and invalid position.
     */
    Token() : type_(TT::UNKNOWN), value_(), position_(-1) {}

    /**
     * @brief Constructs a token with type, value, and position.
     * @param t Token type
     * @param v Token value (lexeme)
     * @param p Position in source (default: -1)
     */
    Token(TT t, std::string v, int p) : type_(t), value_(std::move(v)), position_(p) {}

    /**
     * @brief Constructs a token with type and value.
     * @param t Token type
     * @param v Token value (lexeme)
     */
    Token(TT t, std::string v) : type_(t), value_(std::move(v)), position_(-1) {}

    /**
     * @brief Virtual destructor for proper inheritance.
     */
    virtual ~Token() = default;

    // === Core Accessors ===

    /**
     * @brief Gets the token type.
     * @return The token's type enumeration value
     */
    TT getType() const { return type_; }

    /**
     * @brief Gets the token's lexeme.
     * @return The string value of the token
     */
    const std::string& getValue() const { return value_; }

    /**
     * @brief Gets the token's position in source.
     * @return Position in source or -1 if unknown
     */
    int getPosition() const { return position_; }

    // === Modifiers ===

    /**
     * @brief Sets the token's position in source.
     * @param position New position (-1 for unknown)
     * @throws std::invalid_argument if position < -1
     */
    void setPosition(int position) {
        if (position < -1) {
            throw std::invalid_argument("Token position cannot be less than -1");
        }
        position_ = position;
    }

    /**
     * @brief Sets the token's value.
     * @param value New token value
     */
    virtual void setValue(std::string value) {
        value_ = std::move(value);
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid type and value
     */
    virtual bool isValid() const {
        return type_ != TT::UNKNOWN && !value_.empty();
    }

    /**
     * @brief Checks if token is at valid position.
     * @return true if position is valid (>= 0)
     */
    bool hasValidPosition() const {
        return position_ >= 0;
    }

    // === Comparison ===

    /**
     * @brief Compares tokens for equality.
     * @param other Token to compare with
     * @return true if tokens are equal
     */
    virtual bool equals(const Token& other) const {
        return type_ == other.type_ && value_ == other.value_;
    }

    /**
     * @brief Equality operator.
     * @param other Token to compare with
     * @return true if tokens are equal
     */
    bool operator==(const Token& other) const {
        return equals(other);
    }

    /**
     * @brief Inequality operator.
     * @param other Token to compare with
     * @return true if tokens are not equal
     */
    bool operator!=(const Token& other) const {
        return !equals(other);
    }

    // === Cloning ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new token instance
     */
    virtual std::unique_ptr<Token> clone() const {
        return std::make_unique<Token>(type_, value_, position_);
    }

    // === String Representation ===

    /**
     * @brief Gets string representation of token.
     * @return Formatted string describing the token
     */
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "Token{type=" << static_cast<int>(type_)
            << ", value=\"" << value_ << "\""
            << ", pos=" << position_ << "}";
        return oss.str();
    }

protected:
    TT type_;             ///< Token type (enum value)
    std::string value_;   ///< String value (lexeme) of the token
    int position_;        ///< Position in the source string

    /**
     * @brief Validates token's internal state.
     * @throws std::invalid_argument if token is invalid
     */
    virtual void validate() const {
        if (type_ == TT::UNKNOWN) {
            throw std::invalid_argument("Token has unknown type");
        }
        if (value_.empty() && type_ != TT::END_OF_FILE) {
            throw std::invalid_argument("Token has empty value");
        }
    }
};

/**
 * @class KeywordToken
 * @brief Token representing a SQL keyword.
 * @details
 * Stores a reference to KeywordInfo, which contains metadata about the keyword.
 * Provides validation and comparison methods specific to SQL keywords.
 *
 * @see Token
 * @see KeywordInfo
 */
class KeywordToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a KeywordToken with the given KeywordInfo.
     * @param info Shared pointer to KeywordInfo
     * @throws std::invalid_argument if info is null
     */
    explicit KeywordToken(std::shared_ptr<KeywordInfo> info)
        : Token(TT::KEYWORD, info ? info->lexeme : ""), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs a KeywordToken with the given KeywordInfo and lexeme.
     * @param info Shared pointer to KeywordInfo
     * @param lexeme The keyword text as it appears in the source
     * @throws std::invalid_argument if info is null
     */
    KeywordToken(std::shared_ptr<KeywordInfo> info, std::string lexeme)
        : Token(TT::KEYWORD, std::move(lexeme)), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an empty KeywordToken (invalid/unknown).
     */
    KeywordToken()
        : Token(TT::KEYWORD, "", -1), infoPtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the keyword category.
     * @return KeywordCategory enum value
     */
    KeywordCategory getCategory() const {
        return infoPtr ? infoPtr->category : KeywordCategory::UNKNOWN;
    }

    /**
     * @brief Gets the keyword subcategory.
     * @return Subcategory value as integer
     */
    int getSubKind() const {
        return infoPtr ? infoPtr->subKind : 0;
    }

    /**
     * @brief Checks if keyword belongs to given category.
     * @param category Category to check against
     * @return true if keyword is of specified category
     */
    bool isCategory(KeywordCategory category) const {
        return infoPtr && infoPtr->category == category;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid info and lexeme
     */
    bool isValid() const override {
        return Token::isValid() && infoPtr != nullptr;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (!infoPtr) {
            throw std::invalid_argument("KeywordToken has null info pointer");
        }
        if (infoPtr->lexeme.empty()) {
            throw std::invalid_argument("KeywordToken has empty lexeme");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares keywords for equality.
     * @param other Token to compare with
     * @return true if keywords are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* kw = dynamic_cast<const KeywordToken*>(&other);
        if (!kw) return false;
        return infoPtr && kw->infoPtr &&
            infoPtr->category == kw->infoPtr->category &&
            infoPtr->subKind == kw->infoPtr->subKind;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new KeywordToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!infoPtr) return std::make_unique<KeywordToken>();
        auto newInfo = std::make_shared<KeywordInfo>(*infoPtr);
        auto token = std::make_unique<KeywordToken>(newInfo);
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of keyword token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "KeywordToken{lexeme=\"" << getValue() << "\""
            << ", category=" << static_cast<int>(getCategory())
            << ", subKind=" << getSubKind()
            << ", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    std::shared_ptr<KeywordInfo> infoPtr; ///< Pointer to keyword metadata

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("KeywordToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class FunctionToken
 * @brief Token representing a SQL function.
 * @details
 * Stores a reference to FunctionInfo, which describes the function and its metadata.
 * Provides validation, comparison, and function-specific operations.
 *
 * @see Token
 * @see FunctionInfo
 */
class FunctionToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a FunctionToken with the given FunctionInfo.
     * @param info Shared pointer to FunctionInfo
     * @throws std::invalid_argument if info is null
     */
    explicit FunctionToken(std::shared_ptr<FunctionInfo> info)
        : Token(TT::FUNCTION, info ? info->name : ""), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs a FunctionToken with the given FunctionInfo and name.
     * @param info Shared pointer to FunctionInfo
     * @param name The function name as it appears in the source
     * @throws std::invalid_argument if info is null
     */
    FunctionToken(std::shared_ptr<FunctionInfo> info, std::string name)
        : Token(TT::FUNCTION, std::move(name)), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an empty FunctionToken (invalid/unknown).
     */
    FunctionToken()
        : Token(TT::FUNCTION, "", -1), infoPtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the function category.
     * @return FunctionCategory enum value
     */
    FunctionCategory getCategory() const {
        return infoPtr ? infoPtr->category : FunctionCategory::UNKNOWN;
    }

    /**
     * @brief Gets the function name.
     * @return Function name string
     */
    const std::string& getName() const {
        return getValue();
    }

    /**
     * @brief Gets the number of required arguments.
     * @return Number of required arguments, or -1 if unknown
     */
    int getRequiredArgCount() const {
        return infoPtr ? infoPtr->minArgs : -1;
    }

    /**
     * @brief Gets the maximum number of arguments.
     * @return Maximum number of arguments, or -1 if unlimited
     */
    int getMaxArgCount() const {
        return infoPtr ? infoPtr->maxArgs : -1;
    }

    /**
     * @brief Checks if function belongs to given category.
     * @param category Category to check against
     * @return true if function is of specified category
     */
    bool isCategory(FunctionCategory category) const {
        return infoPtr && infoPtr->category == category;
    }

    /**
     * @brief Checks if function accepts variable arguments.
     * @return true if function accepts variable number of arguments
     */
    bool hasVariableArgs() const {
        return infoPtr && infoPtr->maxArgs == -1;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid info and name
     */
    bool isValid() const override {
        return Token::isValid() && infoPtr != nullptr;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (!infoPtr) {
            throw std::invalid_argument("FunctionToken has null info pointer");
        }
        if (infoPtr->name.empty()) {
            throw std::invalid_argument("FunctionToken has empty name");
        }
        if (infoPtr->minArgs < 0) {
            throw std::invalid_argument("FunctionToken has invalid minimum argument count");
        }
        if (infoPtr->maxArgs != -1 && infoPtr->maxArgs < infoPtr->minArgs) {
            throw std::invalid_argument("FunctionToken has invalid argument count range");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares functions for equality.
     * @param other Token to compare with
     * @return true if functions are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* fn = dynamic_cast<const FunctionToken*>(&other);
        if (!fn) return false;
        return infoPtr && fn->infoPtr &&
            infoPtr->category == fn->infoPtr->category &&
            infoPtr->minArgs == fn->infoPtr->minArgs &&
            infoPtr->maxArgs == fn->infoPtr->maxArgs;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new FunctionToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!infoPtr) return std::make_unique<FunctionToken>();
        auto newInfo = std::make_shared<FunctionInfo>(*infoPtr);
        auto token = std::make_unique<FunctionToken>(newInfo);
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of function token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "FunctionToken{name=\"" << getName() << "\""
            << ", category=" << static_cast<int>(getCategory())
            << ", args=[" << getRequiredArgCount() << ".."
            << (getMaxArgCount() == -1 ? "∞" : std::to_string(getMaxArgCount()))
            << "], pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    std::shared_ptr<FunctionInfo> infoPtr; ///< Pointer to function metadata

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("FunctionToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class IdentifierToken
 * @brief Token representing a SQL identifier (table, column, alias, etc).
 * @details
 * Stores the identifier string and a pointer to IdentifierInfo with semantic information.
 * Provides validation, comparison, and identifier-specific operations.
 *
 * @see Token
 * @see IdentifierInfo
 */
class IdentifierToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs an IdentifierToken with the given string and IdentifierInfo.
     * @param v The identifier string
     * @param info Shared pointer to IdentifierInfo
     * @throws std::invalid_argument if identifier string is empty
     */
    IdentifierToken(std::string v, std::shared_ptr<IdentifierInfo> info)
        : Token(TT::IDENTIFIER, std::move(v)), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an empty IdentifierToken (invalid/unknown).
     */
    IdentifierToken()
        : Token(TT::IDENTIFIER, "", -1), infoPtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the identifier category.
     * @return IdentifierCategory enum value
     */
    IdentifierCategory getCategory() const {
        return infoPtr ? infoPtr->category : IdentifierCategory::UNKNOWN;
    }

    /**
     * @brief Gets the identifier name.
     * @return Identifier string
     */
    const std::string& getName() const {
        return getValue();
    }

    /**
     * @brief Gets the schema name if available.
     * @return Schema name or empty string if not set
     */
    const std::string& getSchema() const {
        return infoPtr ? infoPtr->schema : empty_string_;
    }

    /**
     * @brief Checks if identifier belongs to given category.
     * @param category Category to check against
     * @return true if identifier is of specified category
     */
    bool isCategory(IdentifierCategory category) const {
        return infoPtr && infoPtr->category == category;
    }

    /**
     * @brief Checks if identifier is qualified with schema.
     * @return true if identifier has schema qualification
     */
    bool isQualified() const {
        return infoPtr && !infoPtr->schema.empty();
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid name
     */
    bool isValid() const override {
        return Token::isValid();
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (getValue().empty()) {
            throw std::invalid_argument("IdentifierToken has empty name");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares identifiers for equality.
     * @param other Token to compare with
     * @return true if identifiers are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* id = dynamic_cast<const IdentifierToken*>(&other);
        if (!id) return false;

        // Compare basic properties
        if (getValue() != id->getValue()) return false;

        // If both have info, compare metadata
        if (infoPtr && id->infoPtr) {
            return infoPtr->category == id->infoPtr->category &&
                infoPtr->schema == id->infoPtr->schema;
        }

        // If neither has info, they're equal
        return !infoPtr && !id->infoPtr;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new IdentifierToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!isValid()) return std::make_unique<IdentifierToken>();
        auto newInfo = infoPtr ? std::make_shared<IdentifierInfo>(*infoPtr) : nullptr;
        auto token = std::make_unique<IdentifierToken>(getValue(), newInfo);
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of identifier token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "IdentifierToken{name=\"" << getName() << "\"";
        if (isQualified()) {
            oss << ", schema=\"" << getSchema() << "\"";
        }
        if (infoPtr) {
            oss << ", category=" << static_cast<int>(getCategory());
        }
        oss << ", pos=" << getPosition() << "}";
        return oss.str();
    }

    /**
     * @brief Replace or augment the IdentifierInfo pointer.
     * @param info New IdentifierInfo shared pointer
     */
    void setInfo(std::shared_ptr<IdentifierInfo> info) {
        infoPtr = std::move(info);
    }

    /**
     * @brief Update token category (delegates to infoPtr->setCategory).
     * @param cat New IdentifierCategory
     */
    void updateCategory(IdentifierCategory cat) {
        if (!infoPtr) {
            infoPtr = std::make_shared<IdentifierInfo>();
        }
        infoPtr->setCategory(cat);
    }

    /**
     * @brief Qualify identifier with schema and database.
     * @param schema Schema name
     * @param database Database name (optional)
     */
    void qualify(const std::string& schema, const std::string& database = "") {
        if (!infoPtr) infoPtr = std::make_shared<IdentifierInfo>();
        infoPtr->schema = schema;
        if (!database.empty()) infoPtr->database = database;
    }

public:
    std::shared_ptr<IdentifierInfo> infoPtr; ///< Pointer to identifier metadata

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("IdentifierToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }

private:
    static const std::string empty_string_; ///< Empty string constant for getSchema
};

/**
 * @class LiteralToken
 * @brief Token representing a SQL literal value.
 * @details
 * Stores the literal's category and a pointer to its value object.
 * Provides validation, comparison, and literal-specific operations.
 *
 * @see Token
 * @see LiteralValue
 */
class LiteralToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a LiteralToken with the given category and string value.
     * @param lt The literal category
     * @param v The string value
     * @throws std::invalid_argument if value is empty or category is unknown
     */
    LiteralToken(LiteralCategory lt, std::string v)
        : Token(TT::LITERAL, std::move(v)), litType(lt), valuePtr(nullptr)
    {
        validate();
    }

    /**
     * @brief Constructs a LiteralToken with category, string value, and value object.
     * @param lt The literal category
     * @param v The string value
     * @param val Shared pointer to the value object
     * @throws std::invalid_argument if value is empty or category is unknown
     */
    LiteralToken(LiteralCategory lt, std::string v, std::shared_ptr<LiteralValue> val)
        : Token(TT::LITERAL, std::move(v)), litType(lt), valuePtr(std::move(val))
    {
        validate();
    }

    /**
     * @brief Constructs an empty LiteralToken (invalid/unknown).
     */
    LiteralToken()
        : Token(TT::LITERAL, "", -1), litType(LiteralCategory::UNKNOWN), valuePtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the literal category.
     * @return LiteralCategory enum value
     */
    LiteralCategory getCategory() const {
        return litType;
    }

    /**
     * @brief Gets the raw string value.
     * @return Original literal string
     */
    const std::string& getRawValue() const {
        return Token::getValue();
    }

    /**
     * @brief Gets the parsed value object if available.
     * @return Shared pointer to literal value object
     */
    std::shared_ptr<LiteralValue> getValue() const {
        return valuePtr;
    }

    /**
     * @brief Checks if literal belongs to given category.
     * @param category Category to check against
     * @return true if literal is of specified category
     */
    bool isCategory(LiteralCategory category) const {
        return litType == category;
    }

    /**
     * @brief Checks if literal has parsed value object.
     * @return true if literal has associated value object
     */
    bool hasValue() const {
        return valuePtr != nullptr;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid category and value
     */
    bool isValid() const override {
        return Token::isValid() && litType != LiteralCategory::UNKNOWN;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (litType == LiteralCategory::UNKNOWN) {
            throw std::invalid_argument("LiteralToken has unknown category");
        }
        if (Token::getValue().empty() && litType != LiteralCategory::NULL_VALUE) {
            throw std::invalid_argument("LiteralToken has empty value");
        }
        if (valuePtr && !valuePtr->isValid()) {
            throw std::invalid_argument("LiteralToken has invalid value object");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares literals for equality.
     * @param other Token to compare with
     * @return true if literals are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* lit = dynamic_cast<const LiteralToken*>(&other);
        if (!lit) return false;

        if (litType != lit->litType) return false;
        if (Token::getValue() != lit->Token::getValue()) return false;

        // Compare value objects if both exist
        if (valuePtr && lit->valuePtr) {
            return valuePtr->equals(*lit->valuePtr);
        }

        // If neither has value object, they're equal
        return !valuePtr && !lit->valuePtr;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new LiteralToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!isValid()) return std::make_unique<LiteralToken>();

        if (valuePtr) {
            return std::make_unique<LiteralToken>(
                litType,
                Token::getValue(),
                std::shared_ptr<LiteralValue>(valuePtr->clone())
            );
        }

        auto token = std::make_unique<LiteralToken>(litType, Token::getValue());
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of literal token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "LiteralToken{value=\"" << getRawValue() << "\""
            << ", category=" << static_cast<int>(litType);
        if (hasValue()) {
            oss << ", parsed=" << valuePtr->toString();
        }
        oss << ", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    LiteralCategory litType;                ///< Category of the literal
    std::shared_ptr<LiteralValue> valuePtr; ///< Pointer to the literal value object

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty() && litType != LiteralCategory::NULL_VALUE) {
            throw std::invalid_argument("LiteralToken value cannot be empty except for NULL");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class LiteralCategoryToken
 * @brief Token representing a literal with specific category (e.g., INTEGER, STRING, JSON).
 * @details
 * Used for tokens that carry a LiteralCategory value. Provides validation,
 * comparison, and category-specific logic.
 *
 * @see Token
 * @see LiteralCategory
 */
class LiteralCategoryToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a LiteralCategoryToken.
     * @param c The literal category enum
     * @param v The string lexeme
     * @param p The position in source
     * @throws std::invalid_argument if category is UNKNOWN or value empty
     */
    LiteralCategoryToken(LiteralCategory c, std::string v, int p)
        : Token(TT::LITERAL_CATEGORY, std::move(v), p), category(c)
    {
        validate();
    }

    /**
     * @brief Constructs a LiteralCategoryToken without position.
     * @param c The literal category enum
     * @param v The string lexeme
     * @throws std::invalid_argument if category is UNKNOWN or value empty
     */
    LiteralCategoryToken(LiteralCategory c, std::string v)
        : Token(TT::LITERAL_CATEGORY, std::move(v)), category(c)
    {
        validate();
    }

    /**
     * @brief Default constructor for an invalid/unknown literal.
     */
    LiteralCategoryToken()
        : Token(TT::LITERAL_CATEGORY, "", -1), category(LiteralCategory::UNKNOWN)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the literal category.
     * @return LiteralCategory enum value
     */
    LiteralCategory getCategory() const {
        return category;
    }

    /**
     * @brief Gets the literal lexeme.
     * @return String representation of the literal
     */
    const std::string& getLexeme() const {
        return getValue();
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if category != UNKNOWN and value non-empty
     */
    bool isValid() const override {
        return Token::isValid() && category != LiteralCategory::UNKNOWN;
    }

    /**
     * @brief Validates token's internal state.
     * @throws std::invalid_argument if invalid
     */
    void validate() const override {
        Token::validate();
        if (category == LiteralCategory::UNKNOWN) {
            throw std::invalid_argument("LiteralCategoryToken has unknown category");
        }
        if (getValue().empty()) {
            throw std::invalid_argument("LiteralCategoryToken value cannot be empty");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares literal tokens for equality.
     * @param other Token to compare with
     * @return true if both category and lexeme match
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        auto* lit = dynamic_cast<const LiteralCategoryToken*>(&other);
        return lit && category == lit->category;
    }

    // === Cloning ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new LiteralCategoryToken
     */
    std::unique_ptr<Token> clone() const override {
        if (!isValid()) return std::make_unique<LiteralCategoryToken>();
        auto token = std::make_unique<LiteralCategoryToken>(category, getValue());
        token->setPosition(getPosition());
        return token;
    }

    // === String Representation ===

    /**
     * @brief Gets string representation of literal token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "LiteralCategoryToken{category=" << static_cast<int>(category)
            << ", value='" << getValue() << "'"
            << ", pos=" << getPosition() << "}";
        return oss.str();
    }

protected:
    LiteralCategory category;  ///< The specific literal category
};

/**
 * @class OperatorToken
 * @brief Token representing a SQL operator.
 * @details
 * Stores a reference to OperatorInfo with operator metadata.
 * Provides validation, comparison, and operator-specific operations.
 *
 * @see Token
 * @see OperatorInfo
 */
class OperatorToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs an OperatorToken with the given OperatorInfo.
     * @param info Shared pointer to OperatorInfo
     * @throws std::invalid_argument if info is null
     */
    explicit OperatorToken(std::shared_ptr<OperatorInfo> info)
        : Token(TT::OPERATOR, info ? info->symbol : ""), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an OperatorToken with the given OperatorInfo and symbol.
     * @param info Shared pointer to OperatorInfo
     * @param symbol The operator symbol as it appears in the source
     * @throws std::invalid_argument if info is null
     */
    OperatorToken(std::shared_ptr<OperatorInfo> info, std::string symbol)
        : Token(TT::OPERATOR, std::move(symbol)), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an empty OperatorToken (invalid/unknown).
     */
    OperatorToken()
        : Token(TT::OPERATOR, "", -1), infoPtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the operator category.
     * @return OperatorCategory enum value
     */
    OperatorCategory getCategory() const {
        return infoPtr ? infoPtr->category : OperatorCategory::UNKNOWN;
    }

    /**
     * @brief Gets the operator symbol.
     * @return Operator symbol string
     */
    const std::string& getSymbol() const {
        return getValue();
    }

    /**
     * @brief Gets the operator precedence.
     * @return Precedence value (higher binds tighter)
     */
    int getPrecedence() const {
        return infoPtr ? infoPtr->precedence : 0;
    }

    /**
     * @brief Checks if operator belongs to given category.
     * @param category Category to check against
     * @return true if operator is of specified category
     */
    bool isCategory(OperatorCategory category) const {
        return infoPtr && infoPtr->category == category;
    }

    /**
     * @brief Checks if operator is left associative.
     * @return true if operator is left associative
     */
    bool isLeftAssociative() const {
        return infoPtr && infoPtr->leftAssociative;
    }

    /**
     * @brief Checks if operator is unary.
     * @return true if operator is unary
     */
    bool isUnary() const {
        return infoPtr && infoPtr->unary;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid info and symbol
     */
    bool isValid() const override {
        return Token::isValid() && infoPtr != nullptr;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (!infoPtr) {
            throw std::invalid_argument("OperatorToken has null info pointer");
        }
        if (infoPtr->symbol.empty()) {
            throw std::invalid_argument("OperatorToken has empty symbol");
        }
        if (infoPtr->category == OperatorCategory::UNKNOWN) {
            throw std::invalid_argument("OperatorToken has unknown category");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares operators for equality.
     * @param other Token to compare with
     * @return true if operators are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* op = dynamic_cast<const OperatorToken*>(&other);
        if (!op) return false;
        return infoPtr && op->infoPtr &&
            infoPtr->category == op->infoPtr->category &&
            infoPtr->precedence == op->infoPtr->precedence &&
            infoPtr->leftAssociative == op->infoPtr->leftAssociative &&
            infoPtr->unary == op->infoPtr->unary;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new OperatorToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!infoPtr) return std::make_unique<OperatorToken>();
        auto newInfo = std::make_shared<OperatorInfo>(*infoPtr);
        auto token = std::make_unique<OperatorToken>(newInfo);
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of operator token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "OperatorToken{symbol=\"" << getSymbol() << "\""
            << ", category=" << static_cast<int>(getCategory())
            << ", precedence=" << getPrecedence()
            << ", " << (isLeftAssociative() ? "left" : "right") << "-assoc"
            << (isUnary() ? ", unary" : "")
            << ", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    std::shared_ptr<OperatorInfo> infoPtr; ///< Pointer to operator metadata

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("OperatorToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class DateTimePartToken
 * @brief Token representing a SQL date/time part (e.g. YEAR, MONTH).
 * @details
 * Used for tokens that refer to specific parts of date/time values.
 * Provides validation, comparison, and datetime-specific operations.
 *
 * @see Token
 * @see DateTimePart
 */
class DateTimePartToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a DateTimePartToken.
     * @param p The date/time part (enum)
     * @param v The string representation
     * @throws std::invalid_argument if value is empty or part is unknown
     */
    DateTimePartToken(DateTimePart p, std::string v)
        : Token(TT::DATETIMEPART, std::move(v)), part(p)
    {
        validate();
    }

    /**
     * @brief Constructs an empty DateTimePartToken (invalid/unknown).
     */
    DateTimePartToken()
        : Token(TT::DATETIMEPART, "", -1), part(DateTimePart::UNKNOWN)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the datetime part.
     * @return DateTimePart enum value
     */
    DateTimePart getPart() const {
        return part;
    }

    /**
     * @brief Gets the string representation.
     * @return String representation of the datetime part
     */
    const std::string& getName() const {
        return getValue();
    }

    /**
     * @brief Checks if part is a date component.
     * @return true if part represents a date component
     */
    bool isDatePart() const {
        switch (part) {
        case DateTimePart::YEAR:
        case DateTimePart::QUARTER:
        case DateTimePart::MONTH:
        case DateTimePart::DAY:
        case DateTimePart::DAY_OF_YEAR:
        case DateTimePart::WEEK:
        case DateTimePart::ISO_WEEK:
        case DateTimePart::WEEKDAY:
            return true;
        default:
            return false;
        }
    }

    /**
     * @brief Checks if part is a time component.
     * @return true if part represents a time component
     */
    bool isTimePart() const {
        switch (part) {
        case DateTimePart::HOUR:
        case DateTimePart::MINUTE:
        case DateTimePart::SECOND:
        case DateTimePart::MILLISECOND:
        case DateTimePart::MICROSECOND:
        case DateTimePart::NANOSECOND:
            return true;
        default:
            return false;
        }
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid part and value
     */
    bool isValid() const override {
        return Token::isValid() && part != DateTimePart::UNKNOWN;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (part == DateTimePart::UNKNOWN) {
            throw std::invalid_argument("DateTimePartToken has unknown part");
        }
        if (getValue().empty()) {
            throw std::invalid_argument("DateTimePartToken has empty value");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares datetime parts for equality.
     * @param other Token to compare with
     * @return true if datetime parts are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* dt = dynamic_cast<const DateTimePartToken*>(&other);
        if (!dt) return false;
        return part == dt->part;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new DateTimePartToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!isValid()) return std::make_unique<DateTimePartToken>();
        auto token = std::make_unique<DateTimePartToken>(part, getValue());
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of datetime part token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "DateTimePartToken{part=" << getName()
            << ", type=" << (isDatePart() ? "date" :
                (isTimePart() ? "time" : "zone"))
            << ", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    DateTimePart part; ///< The datetime part represented by this token

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("DateTimePartToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class PunctuatorToken
 * @brief Token representing a SQL punctuator (symbol, delimiter).
 * @details
 * Stores a reference to PunctuatorInfo for metadata.
 * Provides validation, comparison, and punctuator-specific operations.
 *
 * @see Token
 * @see PunctuatorInfo
 */
class PunctuatorToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a PunctuatorToken with the given PunctuatorInfo.
     * @param info Shared pointer to PunctuatorInfo
     * @throws std::invalid_argument if info is null
     */
    explicit PunctuatorToken(std::shared_ptr<PunctuatorInfo> info)
        : Token(TT::PUNCTUATOR, info ? info->lexeme : ""), infoPtr(std::move(info))
    {
        validate();
    }

    /**
     * @brief Constructs an empty PunctuatorToken (invalid/unknown).
     */
    PunctuatorToken()
        : Token(TT::PUNCTUATOR, "", -1), infoPtr(nullptr)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the punctuator lexeme.
     * @return Punctuator string
     */
    const std::string& getLexeme() const {
        return getValue();
    }

    /**
     * @brief Gets the common symbol type if available.
     * @return CommonSymbol enum value or UNKNOWN
     */
    CommonSymbol getCommonSymbol() const {
        return infoPtr ? infoPtr->commonSymbol : CommonSymbol::UNKNOWN;
    }

    /**
     * @brief Gets the TSQL symbol type if available.
     * @return TSQLSymbol enum value or UNKNOWN
     */
    TSQLSymbol getTSQLSymbol() const {
        return infoPtr ? infoPtr->tsqlSymbol : TSQLSymbol::UNKNOWN;
    }

    /**
     * @brief Gets the string delimiter type if available.
     * @return StringDelimiter enum value or UNKNOWN
     */
    StringDelimiter getDelimiterType() const {
        return infoPtr ? infoPtr->stringDelimiter : StringDelimiter::UNKNOWN;
    }

    /**
     * @brief Checks if punctuator is a common symbol.
     * @return true if punctuator is a common symbol
     */
    bool isCommonSymbol() const {
        return infoPtr && infoPtr->commonSymbol != CommonSymbol::UNKNOWN;
    }

    /**
     * @brief Checks if punctuator is a TSQL symbol.
     * @return true if punctuator is a TSQL symbol
     */
    bool isTSQLSymbol() const {
        return infoPtr && infoPtr->tsqlSymbol != TSQLSymbol::UNKNOWN;
    }

    /**
     * @brief Checks if punctuator is a string delimiter.
     * @return true if punctuator is a string delimiter
     */
    bool isStringDelimiter() const {
        return infoPtr && infoPtr->stringDelimiter != StringDelimiter::UNKNOWN;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid info and lexeme
     */
    bool isValid() const override {
        return Token::isValid() && infoPtr != nullptr;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (!infoPtr) {
            throw std::invalid_argument("PunctuatorToken has null info pointer");
        }
        if (infoPtr->lexeme.empty()) {
            throw std::invalid_argument("PunctuatorToken has empty lexeme");
        }
        // At least one symbol type must be valid
        if (infoPtr->commonSymbol == CommonSymbol::UNKNOWN &&
            infoPtr->tsqlSymbol == TSQLSymbol::UNKNOWN &&
            infoPtr->stringDelimiter == StringDelimiter::UNKNOWN) {
            throw std::invalid_argument("PunctuatorToken has no valid symbol type");
        }
    }

    // === Comparison ===

    /**
     * @brief Compares punctuators for equality.
     * @param other Token to compare with
     * @return true if punctuators are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* punct = dynamic_cast<const PunctuatorToken*>(&other);
        if (!punct) return false;
        return infoPtr && punct->infoPtr &&
            infoPtr->commonSymbol == punct->infoPtr->commonSymbol &&
            infoPtr->tsqlSymbol == punct->infoPtr->tsqlSymbol &&
            infoPtr->stringDelimiter == punct->infoPtr->stringDelimiter;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new PunctuatorToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!infoPtr) return std::make_unique<PunctuatorToken>();
        auto newInfo = std::make_shared<PunctuatorInfo>(*infoPtr);
        auto token = std::make_unique<PunctuatorToken>(newInfo);
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of punctuator token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "PunctuatorToken{lexeme=\"" << getLexeme() << "\"";
        if (isCommonSymbol()) {
            oss << ", common=" << static_cast<int>(getCommonSymbol());
        }
        if (isTSQLSymbol()) {
            oss << ", tsql=" << static_cast<int>(getTSQLSymbol());
        }
        if (isStringDelimiter()) {
            oss << ", delim=" << static_cast<int>(getDelimiterType());
        }
        oss << ", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    std::shared_ptr<PunctuatorInfo> infoPtr; ///< Pointer to punctuator metadata

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        if (value.empty()) {
            throw std::invalid_argument("PunctuatorToken value cannot be empty");
        }
        Token::setValue(std::move(value));
    }
};

/**
 * @class CommentToken
 * @brief Token representing a SQL comment.
 * @details
 * Stores the comment type (single-line, multi-line, etc).
 * Provides validation, comparison, and comment-specific operations.
 *
 * @see Token
 * @see CommentType
 */
class CommentToken : public Token {
public:
    // === Constructors ===

    /**
     * @brief Constructs a CommentToken with the given type and value.
     * @param t The comment type (enum)
     * @param v The comment string
     * @throws std::invalid_argument if comment type is unknown
     */
    CommentToken(CommentType t, std::string v)
        : Token(TT::COMMENT, std::move(v)), commentType(t)
    {
        validate();
    }

    /**
     * @brief Constructs an empty CommentToken (invalid/unknown).
     */
    CommentToken()
        : Token(TT::COMMENT, "", -1), commentType(CommentType::UNKNOWN)
    {
    }

    // === Accessors ===

    /**
     * @brief Gets the comment type.
     * @return CommentType enum value
     */
    CommentType getType() const {
        return commentType;
    }

    /**
     * @brief Gets the comment text.
     * @return Comment string without delimiters
     */
    const std::string& getText() const {
        return getValue();
    }

    /**
     * @brief Checks if comment is single-line.
     * @return true if comment is single-line
     */
    bool isSingleLine() const {
        return commentType == CommentType::SINGLE_LINE;
    }

    /**
     * @brief Checks if comment is multi-line.
     * @return true if comment is multi-line
     */
    bool isMultiLine() const {
        return commentType == CommentType::MULTI_LINE;
    }

    /**
     * @brief Gets the comment's line count.
     * @return Number of lines in the comment
     */
    size_t getLineCount() const {
        if (getValue().empty()) return 0;
        return std::count(getValue().begin(), getValue().end(), '\n') + 1;
    }

    // === Validation ===

    /**
     * @brief Checks if token is valid.
     * @return true if token has valid type and content
     */
    bool isValid() const override {
        return Token::isValid() && commentType != CommentType::UNKNOWN;
    }

    /**
     * @brief Validates token's state.
     * @throws std::invalid_argument if token is invalid
     */
    void validate() const override {
        Token::validate();
        if (commentType == CommentType::UNKNOWN) {
            throw std::invalid_argument("CommentToken has unknown type");
        }
        validateCommentSyntax();
    }

    // === Comparison ===

    /**
     * @brief Compares comments for equality.
     * @param other Token to compare with
     * @return true if comments are equal
     */
    bool equals(const Token& other) const override {
        if (!Token::equals(other)) return false;
        const auto* cmt = dynamic_cast<const CommentToken*>(&other);
        if (!cmt) return false;
        return commentType == cmt->commentType;
    }

    // === Conversion ===

    /**
     * @brief Creates a deep copy of the token.
     * @return Unique pointer to new CommentToken instance
     */
    std::unique_ptr<Token> clone() const override {
        if (!isValid()) return std::make_unique<CommentToken>();
        auto token = std::make_unique<CommentToken>(commentType, getValue());
        token->setPosition(getPosition());
        return token;
    }

    /**
     * @brief Gets string representation of comment token.
     * @return Formatted string with token details
     */
    std::string toString() const override {
        std::ostringstream oss;
        oss << "CommentToken{type="
            << (isSingleLine() ? "single-line" : "multi-line")
            << ", lines=" << getLineCount()
            << ", text=\"" << (getValue().length() > 30
                ? getValue().substr(0, 27) + "..."
                : getValue())
            << "\", pos=" << getPosition() << "}";
        return oss.str();
    }

public:
    CommentType commentType; ///< Type of comment (enum)

protected:
    /**
     * @brief Sets the token's value with validation.
     * @param value New token value
     * @throws std::invalid_argument if value is invalid
     */
    void setValue(std::string value) override {
        Token::setValue(std::move(value));
        validateCommentSyntax();
    }

private:
    /**
     * @brief Validates comment syntax based on type.
     * @throws std::invalid_argument if syntax is invalid for the comment type
     */
    void validateCommentSyntax() const {
        if (getValue().empty()) return;

        switch (commentType) {
        case CommentType::SINGLE_LINE:
            if (getValue().find('\n') != std::string::npos) {
                throw std::invalid_argument("Single-line comment contains newline");
            }
            if (getValue().substr(0, 2) != "--") {
                throw std::invalid_argument("Single-line comment must start with --");
            }
            break;

        case CommentType::MULTI_LINE:
            if (getValue().length() < 4 ||
                getValue().substr(0, 2) != "/*" ||
                getValue().substr(getValue().length() - 2) != "*/") {
                throw std::invalid_argument("Invalid multi-line comment syntax");
            }
            break;

        case CommentType::UNKNOWN:
            throw std::invalid_argument("Unknown comment type");
        }
    }
};