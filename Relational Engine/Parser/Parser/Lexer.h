#pragma once

#include "Token.h"
#include <string>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>

class Lexer {
private:
    std::string input;
    size_t pos;
    static const std::unordered_map<std::string, Keyword> keywordMap;

    static std::unordered_map<std::string, Keyword> createKeywordMap();

    // Token type 
    TokenType determineTokenType(const size_t& start);
    TokenType determineTokenType(const std::string& word);
    // Keyword type 
    Keyword determineKeywordType(const size_t& start);
    Keyword determineKeywordType(const std::string& word);
    // Literal type 
    LiteralType determineLiteralType(const size_t& start);
    LiteralType determineLiteralType(const std::string& word);
    // Operator type
    Operator determineOperatorType(const size_t& start);
    Operator determineOperatorType(const std::string& word);
    // Symbol type
    Symbol determineSymbolType(const size_t& start);
    Symbol determineSymbolType(const std::string& word);

    void skipWhitespace();

    // Literal types chech
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

public:
    explicit Lexer(const std::string& query);
    static Keyword getKeyword(const std::string& word);
    std::unique_ptr<Token> nextToken();
    std::unique_ptr<Token> peekToken();

};