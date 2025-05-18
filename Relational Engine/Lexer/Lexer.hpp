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
 * @brief SQL Lexer: Tokenizes an input SQL query string into SQL tokens.
 *
 * Usage:
 *   Lexer lexer("SELECT * FROM table WHERE id = 1;");
 *   auto token = lexer.nextToken();
 *   // process tokens...
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
    bool isIdentifierStart(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '@';
    }

    /**
     * @brief Returns true if @c c can be part of an identifier (after the first char).
     */
    bool isIdentifierPart(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    // === Lexing Methods (each returns a Token if it matches) ===

    std::unique_ptr<Token> lexComment();
    std::unique_ptr<Token> lexDotOperator();
    std::unique_ptr<Token> lexNumber();
    std::unique_ptr<Token> lexKeyword();
    std::unique_ptr<Token> lexFunction();
    std::unique_ptr<Token> lexIdentifier();
    std::unique_ptr<Token> lexDelimitedIdentifier();
    std::unique_ptr<Token> lexString();
    std::unique_ptr<Token> lexJson();
    std::unique_ptr<Token> lexOperator();
    std::unique_ptr<Token> lexPunctuator();

    // === Literal Checking Methods ===

    bool isStringLiteral(const std::string& word);
    bool isCharLiteral(const std::string& word);
    bool isIntegerLiteral(const std::string& word);
    bool isFloatLiteral(const std::string& word);
    bool isBinaryLiteral(const std::string& word);
    bool isHexLiteral(const std::string& word);
    bool isDateLiteral(const std::string& word);
    bool isTimeLiteral(const std::string& word);
    bool isDateTimeLiteral(const std::string& word);
    bool isJSONLiteral(const std::string& word);
    bool isBooleanLiteral(const std::string& word);
    bool isNullLiteral(const std::string& word);

    /**
     * @brief Returns true if @c c is a valid operator character.
     */
    bool isOperatorChar(char c);

    // === Token Creation Methods (maps string to token kind) ===

    std::unique_ptr<Token> createKeywordToken(const std::string& word);
    std::unique_ptr<Token> createFunctionToken(const std::string& word);
    std::unique_ptr<Token> createLiteralToken(const std::string& word);
    std::unique_ptr<Token> createOperatorToken(const std::string& word);
    std::unique_ptr<Token> createPunctuatorToken(const std::string& word);

    // === Keyword, Function, Operator, and Symbol Maps ===

    static const std::unordered_map<std::string, DMLKeyword> dmlMap;
    static const std::unordered_map<std::string, DDLKeyword> ddlMap;
    static const std::unordered_map<std::string, ClauseKeyword> clauseMap;
    static const std::unordered_map<std::string, CTEKeyword> cteMap;
    static const std::unordered_map<std::string, SetOpKeyword> setOpMap;
    static const std::unordered_map<std::string, WordOperatorKeyword> wordOpMap;
    static const std::unordered_map<std::string, LogicalConstantKeyword> logicalConstMap;
    static const std::unordered_map<std::string, TransactionKeyword> tranMap;
    static const std::unordered_map<std::string, SecurityKeyword> secMap;
    static const std::unordered_map<std::string, ProgStmtKeyword> progMap;
    static const std::unordered_map<std::string, MiscKeyword> miscMap;

    static const std::unordered_map<std::string, AggregateFunction> aggFuncMap;
    static const std::unordered_map<std::string, ScalarFunction> scalarFuncMap;
    static const std::unordered_map<std::string, StringFunction> stringFuncMap;
    static const std::unordered_map<std::string, DateTimeFunction> dtFuncMap;
    static const std::unordered_map<std::string, MathFunction> mathFuncMap;
    static const std::unordered_map<std::string, SystemFunction> sysFuncMap;

    static const std::unordered_map<std::string, ArithmeticOp> arithMap;
    static const std::unordered_map<std::string, AssignOp> assignMap;
    static const std::unordered_map<std::string, ComparisonOp> compMap;
    static const std::unordered_map<std::string, LogicalOp> logMap;
    static const std::unordered_map<std::string, BitwiseOp> bitMap;
    static const std::unordered_map<std::string, ConcatOp> concatMap;

    static const std::unordered_map<char, CommonSymbol> commonSymMap;
    static const std::unordered_map<std::string, TSQLSymbol> tsqlSymMap;
    static const std::unordered_map<char, StringDelimiter> strDelimMap;

    static const std::unordered_map<std::string, DateTimePart> dateTimePartsMap;
};