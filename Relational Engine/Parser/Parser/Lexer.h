#pragma once

#include "Keyword.h"
#include "Token.h"
#include "literal.h"
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
    TokenType determineTokenType(const size_t& start);
    TokenType determineTokenType(const std::string& word);

    TokenType determineKeywordType(const size_t& start);
    TokenType determineKeywordType(const std::string& word);

    void skipWhitespace();

    // keyword or Identifier reading methods
    std::unique_ptr<Token> readKeywordOrIdentifier();

    // Literal reading methods
    std::unique_ptr<Token> readStringLiteral();   // For STRING: "..."
    std::unique_ptr<Token> readCharLiteral();     // For CHAR: '...'
    std::unique_ptr<Token> readIntegerLiteral();  // For INTEGER: 123, -456
    std::unique_ptr<Token> readFloatLiteral();    // For FLOAT: 3.14, -0.001
    std::unique_ptr<Token> readBinaryLiteral();   // For BINARY: 0101
    std::unique_ptr<Token> readHexLiteral();      // For HEX: X'01AF'
    std::unique_ptr<Token> readDateLiteral();     // For DATE: 'YYYY-MM-DD'
    std::unique_ptr<Token> readTimeLiteral();     // For TIME: 'HH:MM:SS'
    std::unique_ptr<Token> readDateTimeLiteral(); // For DATETIME: 'YYYY-MM-DD HH:MM:SS'
    std::unique_ptr<Token> readJSONLiteral();     // For JSON: { "key": "value" }
    std::unique_ptr<Token> readBooleanLiteral();  // For BOOLEAN: TRUE, FALSE
    std::unique_ptr<Token> readNullLiteral();

public:
    explicit Lexer(const std::string& query);
    static Keyword getKeyword(const std::string& word);
    std::unique_ptr<Token> nextToken();
    std::unique_ptr<Token> peekToken();

};

