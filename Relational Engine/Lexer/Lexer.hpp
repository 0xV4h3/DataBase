#pragma once

#include "Token.hpp"
#include <string>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>

/**
 * @file Lexer.hpp
 * @brief SQL Lexer: Tokenizes an input SQL query string into SQL tokens.
 * @details
 * - Supports all SQL tokens, keywords, identifiers, literals (including escape string, array, UUID, XML, etc.), operators, punctuators, and comments.
 * - Designed for extensibility and reliable integration with Token classes and enums.
 * - Follows extended Doxygen documentation style.
 *
 * Usage example:
 * @code
 *   Lexer lexer("SELECT * FROM table WHERE id = 1;");
 *   auto token = lexer.nextToken();
 *   // process tokens...
 * @endcode
 */
class Lexer {
public:
    /**
     * @brief Construct a Lexer for the given SQL input.
     * @param query The SQL query string to lex.
     */
    explicit Lexer(const std::string& query);

    // === Main Tokenization Methods ===

    /**
     * @brief Returns the next token from the input stream.
     * @return Unique pointer to the next Token.
     */
    std::unique_ptr<Token> nextToken();

    /**
     * @brief Returns the next token without advancing the position.
     * @return Unique pointer to the next Token.
     */
    std::unique_ptr<Token> peekToken();

    /**
     * @brief Jumps to a token at a specific offset from the current token.
     * @param offset Number of tokens to jump ahead (+) or back (-).
     * @return Unique pointer to the Token at the new location.
     */
    std::unique_ptr<Token> jumpToken(int offset);

private:
    std::string input; ///< The raw input query string.
    size_t      pos;   ///< Current position in the input string.
    size_t      tokenIndex; ///< Current index in the token buffer.
    std::vector<std::unique_ptr<Token>> tokens; ///< Token buffer for lookahead/backtracking.

    // === Utility Methods ===

    /**
     * @brief Advances @c pos past any whitespace and newlines.
     */
    void skipWhitespaceAndNewlines();

    /**
     * @brief Returns true if @c c can start an identifier.
     */
    bool isIdentifierStart(char c);

    /**
     * @brief Returns true if @c c can be part of an identifier (after the first char).
     */
    bool isIdentifierPart(char c);

    // === Dollar-Quoted String Lexing ===

    /**
     * @brief Checks if the input at the current text starts with a dollar-quote ($...$).
     *        If so, returns true and sets tag to the full opening delimiter (e.g. "$abc$").
     *        Otherwise, returns false and tag is unchanged.
     * @param text Substring of input beginning at current position.
     * @param tag [out] Set to the tag if found.
     * @return true if dollar-quote start is detected.
     */
    bool isDollarQuoteStart(const std::string& text, std::string& tag);

    /**
     * @brief Detects at pos if this is a dollar-quoted string, and if so, extracts the tag.
     *        Advances pos if found. Returns true if found.
     * @param tag [out] Set to the tag if found.
     * @return true if a dollar-quote tag is detected.
     */
    bool detectDollarQuoteTag(std::string& tag);

    /**
     * @brief Lexes a dollar-quoted string literal ($tag$...$tag$).
     *        Assumes pos is at the opening '$'.
     * @return LiteralToken
     */
    std::unique_ptr<Token> lexDollarQuotedString();

    // === Lexing Methods (each returns a Token if it matches) ===

    /**
     * @brief Lexes SQL comments (single-line and multi-line).
     */
    std::unique_ptr<Token> lexComment();

    /**
     * @brief Lexes dot notation (e.g., C.CustomerID).
     */
    std::unique_ptr<Token> lexDotOperator();

    /**
     * @brief Lexes numeric literals (integer/float).
     */
    std::unique_ptr<Token> lexNumber();

    /**
     * @brief Lexes SQL keywords, returns KeywordToken or LiteralToken for boolean/null.
     */
    std::unique_ptr<Token> lexKeyword();

    /**
     * @brief Lexes SQL functions.
     */
    std::unique_ptr<Token> lexFunction();

    /**
     * @brief Lexes regular, delimited, and special identifiers.
     */
    std::unique_ptr<Token> lexIdentifier();

    /**
     * @brief Lexes delimited identifier ([foo] or "foo").
     */
    std::unique_ptr<Token> lexDelimitedIdentifier();

    /**
     * @brief Lexes string, escape string, binary, hex, date, time, datetime literals (and dollar-quoted strings).
     */
    std::unique_ptr<Token> lexString();

    /**
     * @brief Lexes JSON literal: {...}
     */
    std::unique_ptr<Token> lexJson();

    /**
     * @brief Lexes SQL operators.
     */
    std::unique_ptr<Token> lexOperator();

    /**
     * @brief Lexes punctuators (comma, paren, etc.).
     */
    std::unique_ptr<Token> lexPunctuator();

    // === Literal Checking Methods ===

    /**
     * @brief Checks if a string is a standard string literal.
     */
    bool isStringLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a character literal.
     */
    bool isCharLiteral(const std::string& word);

    /**
     * @brief Checks if a string is an integer literal.
     */
    bool isIntegerLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a float literal.
     */
    bool isFloatLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a binary literal.
     */
    bool isBinaryLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a hex literal.
     */
    bool isHexLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a date literal.
     */
    bool isDateLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a time literal.
     */
    bool isTimeLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a datetime literal.
     */
    bool isDateTimeLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a JSON literal.
     */
    bool isJSONLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a boolean literal.
     */
    bool isBooleanLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a NULL literal.
     */
    bool isNullLiteral(const std::string& word);

    /**
     * @brief Checks if a string is an escape string literal (E'...').
     */
    bool isEscapeStringLiteral(const std::string& word);

    /**
     * @brief Checks if a string is an array literal ([...]).
     */
    bool isArrayLiteral(const std::string& word);

    /**
     * @brief Checks if a string is a UUID literal.
     */
    bool isUUIDLiteral(const std::string& word);

    /**
     * @brief Checks if a string is an XML literal.
     */
    bool isXMLLiteral(const std::string& word);

    /**
     * @brief Returns true if @c c is a valid operator character.
     */
    bool isOperatorChar(char c);

    // === Token Creation Methods (maps string to token kind) ===

    /**
     * @brief Creates a KeywordToken from a string, or nullptr if not a keyword.
     */
    std::unique_ptr<Token> createKeywordToken(const std::string& word);

    /**
     * @brief Creates a FunctionToken from a string, or nullptr if not a function.
     */
    std::unique_ptr<Token> createFunctionToken(const std::string& word);

    /**
     * @brief Creates a LiteralToken of the appropriate type from a string.
     */
    std::unique_ptr<Token> createLiteralToken(const std::string& word);

    /**
     * @brief Creates an OperatorToken from a string, or UNKNOWN if not a known operator.
     */
    std::unique_ptr<Token> createOperatorToken(const std::string& word);

    /**
     * @brief Creates a PunctuatorToken from a string, or UNKNOWN if not a known punctuator.
     */
    std::unique_ptr<Token> createPunctuatorToken(const std::string& word);

    // === Keyword, Function, Operator, and Symbol Info Maps ===

    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> dmlMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> ddlMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> clauseMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> cteMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> setOpMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> wordOpMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> logicalConstMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> tranMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> secMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> progMap;
    static const std::unordered_map<std::string, std::shared_ptr<KeywordInfo>> miscMap;

    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> aggFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> scalarFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> stringFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> dtFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> mathFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> sysFuncMap;
    static const std::unordered_map<std::string, std::shared_ptr<FunctionInfo>> windowFuncMap;

    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> arithMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> assignMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> compMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> logMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> bitMap;
    static const std::unordered_map<std::string, std::shared_ptr<OperatorInfo>> concatMap;

    static const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> commonSymMap;
    static const std::unordered_map<std::string, std::shared_ptr<PunctuatorInfo>> tsqlSymMap;
    static const std::unordered_map<char, std::shared_ptr<PunctuatorInfo>> strDelimMap;

    static const std::unordered_map<std::string, DateTimePart> dateTimePartsMap;
};