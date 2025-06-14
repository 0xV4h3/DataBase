/**
 * @file Lexer.hpp
 * @brief SQL Lexer with advanced token support and external library integrations.
 */

#pragma once

 // Core includes
#include "Token.hpp"
#include <string>
#include <sstream>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

// External library includes
#include <json.hpp>
#include <pugixml.hpp>
#include <uuid.h>

/**
 * @brief Configuration options for Lexer behavior
 */
struct LexerConfig {
    bool allowDollarQuotes = true;    ///< Allow dollar-quoted strings
    bool allowXMLLiterals = true;     ///< Allow XML literals
    bool allowJSONLiterals = true;    ///< Allow JSON literals
    bool allowUUIDLiterals = true;    ///< Allow UUID literals
    bool allowArrayLiterals = true;    ///< Allow array literals
    bool caseSensitive = false;       ///< Case-sensitive lexing
    bool strictMode = true;           ///< Strict syntax checking
    size_t maxStringLength = 1048576; ///< Max string literal length
    size_t maxIdentLength = 256;      ///< Max identifier length
};

/**
 * @brief Error severity levels for the lexer
 */
enum class ErrorSeverity {
    INFO,    ///< Informational messages
    WARNING, ///< Warning conditions
    ERROR,   ///< Error conditions
    FATAL    ///< Fatal errors
};

/**
 * @brief Structure to hold detailed error information
 */
struct ErrorInfo {
    std::string message;    ///< Error message
    size_t position;        ///< Position in input
    size_t line;           ///< Line number
    size_t column;         ///< Column number
    ErrorSeverity severity; ///< Error severity
    std::string timestamp; ///< Error timestamp
    std::string context;   ///< Context of the error
    std::string pointer;   ///< Visual pointer to error location
};

/**
 * @brief Custom exception for Lexer errors
 */
class LexerError : public std::runtime_error {
public:
    LexerError(const std::string& msg, size_t pos, const std::string& token = "")
        : std::runtime_error(msg), position(pos), token(token) {}

    size_t getPosition() const { return position; }
    const std::string& getToken() const { return token; }

private:
    size_t position;      ///< Error position in input
    std::string token;    ///< Problematic token if any
};

/**
 * @brief SQL Lexer with advanced token support
 */
class Lexer {
public:
    // === Public Interface ===
    explicit Lexer(const std::string& query, const LexerConfig& config = LexerConfig());

    // Core Interface
    std::unique_ptr<Token> nextToken();
    std::unique_ptr<Token> peekToken();
    std::unique_ptr<Token> jumpToken(int offset);

    // State Access
    size_t getCurrentPosition() const { return pos; }
    size_t getCurrentLine() const { return line; }
    size_t getCurrentColumn() const { return column; }
    size_t getCurrentTokenIndex() const;
    size_t getTokenCount() const;
    void reset();

    // Error Handling Interface
    const std::string& getErrorContext() const { return errorContext; }
    bool hasErrors(ErrorSeverity minSeverity = ErrorSeverity::ERROR) const;
    const std::vector<ErrorInfo>& getErrorLog() const;
    void clearErrorLog();
    std::string getDiagnostics() const;

    // Literal Classification Interface
    bool isEscapeStringLiteral(const std::string& word) const;
    bool isBinaryLiteral(const std::string& word) const;
    bool isHexLiteral(const std::string& word) const;
    bool isDateTimeLiteral(const std::string& word) const;
    bool isDateLiteral(const std::string& word) const;
    bool isTimeLiteral(const std::string& word) const;
    bool isJSONLiteral(const std::string& word) const;
    bool isBooleanLiteral(const std::string& word) const;
    bool isNullLiteral(const std::string& word) const;
    bool isFloatLiteral(const std::string& word) const;
    bool isIntegerLiteral(const std::string& word) const;
    bool isCharLiteral(const std::string& word) const;
    bool isStringLiteral(const std::string& word) const;
    bool isArrayLiteral(const std::string& word) const;
    bool isUUIDLiteral(const std::string& word) const;
    bool isXMLLiteral(const std::string& word) const;

private:
    // === Internal Types ===
    struct Position {
        size_t pos;
        size_t line;
        size_t column;
    };

    // === Internal State ===
    std::string input;                               ///< Input SQL query
    size_t pos;                                      ///< Current position
    size_t line;                                     ///< Current line number
    size_t column;                                   ///< Current column number
    size_t tokenIndex;                               ///< Current token index
    std::string errorContext;                        ///< Context for error reporting
    LexerConfig config;                              ///< Lexer configuration
    std::vector<std::unique_ptr<Token>> tokens;      ///< Token buffer
    std::vector<ErrorInfo> errorLog;                 ///< Error log

    // === Position Tracking ===
    void advance(size_t count = 1);
    void newline();
    void skipWhitespaceAndNewlines();
    Position savePosition() const;
    void restorePosition(const Position& position);

    // === Core Lexing Methods ===
    std::unique_ptr<Token> lexNextToken();
    std::unique_ptr<Token> lexComment();
    std::unique_ptr<Token> lexIdentifier();
    std::unique_ptr<Token> lexDelimitedIdentifier();
    std::unique_ptr<Token> lexOperator(bool isWordOp);
    std::unique_ptr<Token> lexPunctuator();
    std::unique_ptr<Token> lexKeyword();
    std::unique_ptr<Token> lexFunction();
    std::unique_ptr<Token> lexDateTimePart();
    std::unique_ptr<Token> lexLiteralCategory();
    std::unique_ptr<Token> lexDotOperator();

    // === Literal Lexing Methods ===
    std::string lexStringContent(bool allowEscape);
    std::unique_ptr<Token> lexString();
    std::unique_ptr<Token> lexEscapeString();
    std::unique_ptr<Token> lexCharacter();
    void processSign(size_t& start, bool& isNegative);
    std::unique_ptr<Token> lexNumber();
    std::unique_ptr<Token> lexCStyleBinary();
    std::unique_ptr<Token> lexCStyleHex();
    std::unique_ptr<Token> lexBinary();
    std::unique_ptr<Token> lexHex();
    std::unique_ptr<Token> lexBoolean();
    std::unique_ptr<Token> lexNull();
    std::unique_ptr<Token> lexDate();
    std::unique_ptr<Token> lexTime();
    std::unique_ptr<Token> lexDateTime();
    std::unique_ptr<Token> lexInterval();
    std::unique_ptr<Token> lexArray();
    std::vector<std::shared_ptr<LiteralValue>> parseArrayElements(const std::string& arrayStr);
    std::unique_ptr<Token> lexUUID();
    std::unique_ptr<Token> lexJSON();
    std::unique_ptr<Token> lexXML();

    // === Dollar Quote Support ===
    std::unique_ptr<Token> lexDollarQuotedString();
    std::unique_ptr<Token> createDollarQuotedStringLiteral(const std::string& original);
    bool isDollarQuoteStart() const;
    bool extractDollarQuoteTag(std::string& tag);
    bool validateDollarQuoteTag(const std::string& tagContent) const;
    bool isDollarQuotedString(const std::string& text) const;
    std::string processNestedDollarQuotes(const std::string& outerTag);
    std::string processDollarQuoteSpecials(const std::string& text);
    size_t getDollarQuoteLineNumber(size_t startPos, size_t targetPos) const;
    std::string getDollarQuoteErrorContext(size_t position, const std::string& tag) const;

    // === Token Creation ===
    std::unique_ptr<Token> createKeywordToken(const std::string& word);
    std::unique_ptr<Token> createFunctionToken(const std::string& word);
    std::unique_ptr<Token> createDateTimePartToken(const std::string& word);
    std::unique_ptr<Token> createLiteralToken(const std::string& word);
    std::unique_ptr<Token> createLiteralCategoryToken(const std::string& word);
    std::unique_ptr<Token> createOperatorToken(const std::string& word);
    std::unique_ptr<Token> createPunctuatorToken(const std::string& word);
    std::unique_ptr<Token> createIdentifierToken(const std::string& identifier, IdentifierCategory category);
    std::unique_ptr<Token> createDelimitedIdentifierToken(const std::string& identifier, IdentifierCategory category);
    std::unique_ptr<Token> createCommentToken(const std::string& text, CommentType type);

    // === Literal Creation Helpers ===
    std::unique_ptr<Token> createStringLiteral(const std::string& value);
    std::unique_ptr<Token> createEscapeStringLiteral(const std::string& value);
    std::unique_ptr<Token> createCharLiteral(char value);
    std::unique_ptr<Token> createIntegerLiteral(int64_t value);
    std::unique_ptr<Token> createFloatLiteral(double value);
    std::unique_ptr<Token> createBinaryLiteral(const std::string& value);
    std::unique_ptr<Token> createHexLiteral(const std::string& value);
    std::unique_ptr<Token> createBooleanLiteral(bool value);
    std::unique_ptr<Token> createNullLiteral();
    std::unique_ptr<Token> createDateLiteral(const std::string& value);
    std::unique_ptr<Token> createTimeLiteral(const std::string& value);
    std::unique_ptr<Token> createDateTimeLiteral(const std::string& value);
    std::unique_ptr<Token> createStandardIntervalLiteral(const std::string& value, const std::string& fromUnitStr, const std::string& toUnitStr = "");
    std::unique_ptr<Token> createPostgresIntervalLiteral(const std::string& value);
    std::unique_ptr<Token> createArrayLiteral(const std::vector<std::shared_ptr<LiteralValue>>& elems);
    std::unique_ptr<Token> createUUIDLiteral(const std::string& value);
    std::unique_ptr<Token> createJSONLiteral(const std::string& value);
    std::unique_ptr<Token> createXMLLiteral(const std::string& value);

    // === Character Classification ===
    bool isWordStart(char c) const;
    bool isWordPart(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isOperatorChar(char c) const;
    bool isStringDelimiter(char c) const;
    bool isNumberStart(char c) const;
    bool isHexDigit(char c) const;
    bool isBinaryDigit(char c) const;
    bool isDecimalPoint(char c) const;
    bool isExponentMarker(char c) const;
    bool isSign(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isSpecialNameChar(char c) const;
    bool isQuotedIdentifierChar(char c) const;
    bool needsEscaping(char c) const;
    bool isAtEnd() const;
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    std::string peekQuotedLiteralContent(size_t start) const;

    // === String Processing ===
    std::string getEscapedChar(char c) const;
    std::string getUnescapedChar(const std::string& sequence) const;
    std::string convertCodepointToUTF8(int codepoint) const;

    // === Validation Methods ===
    bool validateIdentifier(const std::string& ident) const;
    bool validateDelimitedIdentifier(const std::string& ident) const;
    bool validateString(const std::string& str) const;
    bool validateNumber(const std::string& num) const;
    bool validateCStyleBinary(const std::string& binLiteral) const;
    bool validateCStyleHex(const std::string& hexLiteral) const;
    bool validateBinary(const std::string& binLiteral) const;
    bool validateHex(const std::string& hexLiteral) const;
    bool validateDate(const std::string& date) const;
    bool validateTime(const std::string& time) const;
    bool validateDateTime(const std::string& datetime) const;
    bool validateUUID(const std::string& uuid) const;
    bool validateJSON(const std::string& json) const;
    bool validateXML(const std::string& xml) const;
    bool validateInterval(const std::string& interval) const;
    bool validateStandardInterval(const std::string& value, const std::string& fromUnitStr, const std::string& toUnitStr) const;
    bool validatePostgresInterval(const std::string& value) const;
    bool validateArray(const std::string& array) const;

    // === Error Handling ===
    [[noreturn]] void throwError(const std::string& message, const std::string& token = "") const;
    void setErrorContext(const std::string& context) { errorContext = context; }
    std::string getSourceLine() const;
    std::string getPositionPointer() const;
    std::string getCurrentTimestamp() const;
    void recordError(const std::string& message, ErrorSeverity severity);

    // === Static Maps ===
    // Keyword Maps
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> dmlMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> ddlMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> clauseMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> cteMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> setOpMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> sessionOptionMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> predicateMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> logicalConstMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> tranMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> secMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> progMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> miscMap;

    // Function Maps
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> aggFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> scalarFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> stringFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> dtFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> mathFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> sysFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> windowFuncMap;

    // Operator Maps
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> arithMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> assignMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> compMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> logMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> bitMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> concatMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> jsonOpMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> regexOpMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> typecastOpMap;

    // Punctuator Maps
    static const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> punctuatorMap;
    static const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> tsqlSymMap;
    static const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> strDelimMap;

    // Date/Time Map
    static const std::unordered_map<std::string, DateTimePart> dateTimePartsMap;

    // LiteralCategory Map
    static const std::unordered_map<std::string, LiteralCategory> literalCategoryMap;
};