/**
 * @file TokenUtils.hpp
 * @brief Utility class for extracting display strings from specialized token types.
 */

#pragma once
#include "Token.hpp"
#include "LexerUtils.hpp"
#include <string>

class TokenUtils {
public:
    // === Keyword Token Utilities ===
    static std::string getKeywordClassName(const KeywordToken& token) noexcept;
    static std::string getKeywordName(const KeywordToken& token) noexcept;
    static std::string getKeywordLexeme(const KeywordToken& token) noexcept;

    // === Function Token Utilities ===
    static std::string getFunctionClassName(const FunctionToken& token) noexcept;
    static std::string getFunctionName(const FunctionToken& token) noexcept;
    static std::string getFunctionInfo(const FunctionToken& token) noexcept;

    // === Identifier Token Utilities ===
    static std::string getIdentifierClassName(const IdentifierToken& token) noexcept;
    static std::string getIdentifierName(const IdentifierToken& token) noexcept;
    static std::string getFullyQualifiedName(const IdentifierToken& token) noexcept;

    // === Operator Token Utilities ===
    static std::string getOperatorClassName(const OperatorToken& token) noexcept;
    static std::string getOperatorName(const OperatorToken& token) noexcept;
    static std::string getOperatorSymbol(const OperatorToken& token) noexcept;
    static std::string getOperatorInfo(const OperatorToken& token) noexcept;

    // === Punctuator Token Utilities ===
    static std::string getPunctuatorClassName(const PunctuatorToken& token) noexcept;
    static std::string getPunctuatorName(const PunctuatorToken& token) noexcept;
    static std::string getPunctuatorSymbol(const PunctuatorToken& token) noexcept;
    static std::string getPunctuatorInfo(const PunctuatorToken& token) noexcept;

private:
    TokenUtils() = delete;  
    ~TokenUtils() = delete; 
};