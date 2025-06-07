/**
 * @file TokenUtils.cpp
 * @brief Implementation of TokenUtils methods.
 */

#include "TokenUtils.hpp"

 // === Keyword Token Utilities ===

std::string TokenUtils::getKeywordClassName(
    const KeywordToken& token) noexcept
{
    if (token.infoPtr) {
        return LexerUtils::keywordCategoryToString(token.infoPtr->category);
    }
    return "UNKNOWN";
}

std::string TokenUtils::getKeywordName(
    const KeywordToken& token) noexcept
{
    if (!token.infoPtr) return "UNKNOWN";

    switch (token.infoPtr->category) {
    case KeywordCategory::DML:
        return LexerUtils::DMLKeywordTypeToString(
            static_cast<DMLKeyword>(token.infoPtr->subKind));
    case KeywordCategory::DDL:
        return LexerUtils::DDLKeywordTypeToString(
            static_cast<DDLKeyword>(token.infoPtr->subKind));
    case KeywordCategory::CLAUSE:
        return LexerUtils::ClauseKeywordTypeToString(
            static_cast<ClauseKeyword>(token.infoPtr->subKind));
    case KeywordCategory::CTE:
        return LexerUtils::CTEKeywordTypeToString(
            static_cast<CTEKeyword>(token.infoPtr->subKind));
    case KeywordCategory::SETOP:
        return LexerUtils::SetOpKeywordTypeToString(
            static_cast<SetOpKeyword>(token.infoPtr->subKind));
    case KeywordCategory::PREDICATE:
        return LexerUtils::PredicateKeywordTypeToString(
            static_cast<PredicateKeyword>(token.infoPtr->subKind));
    case KeywordCategory::LOGICAL_CONST:
        return LexerUtils::LogicalConstantKeywordTypeToString(
            static_cast<LogicalConstantKeyword>(token.infoPtr->subKind));
    case KeywordCategory::TRANSACTION:
        return LexerUtils::TransactionKeywordTypeToString(
            static_cast<TransactionKeyword>(token.infoPtr->subKind));
    case KeywordCategory::SECURITY:
        return LexerUtils::SecurityKeywordTypeToString(
            static_cast<SecurityKeyword>(token.infoPtr->subKind));
    case KeywordCategory::PROG_STMT:
        return LexerUtils::ProgStmtKeywordTypeToString(
            static_cast<ProgStmtKeyword>(token.infoPtr->subKind));
    case KeywordCategory::MISC:
        return LexerUtils::MiscKeywordTypeToString(
            static_cast<MiscKeyword>(token.infoPtr->subKind));
    default:
        return "UNKNOWN";
    }
}

std::string TokenUtils::getKeywordLexeme(
    const KeywordToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->lexeme : "UNKNOWN";
}

// === Function Token Utilities ===

std::string TokenUtils::getFunctionClassName(
    const FunctionToken& token) noexcept
{
    if (token.infoPtr) {
        return LexerUtils::FunctionCategoryTypeToString(token.infoPtr->category);
    }
    return "UNKNOWN";
}

std::string TokenUtils::getFunctionName(
    const FunctionToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->name : "UNKNOWN";
}

std::string TokenUtils::getFunctionInfo(
    const FunctionToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->toString() : "UNKNOWN";
}

// === Identifier Token Utilities ===

std::string TokenUtils::getIdentifierClassName(
    const IdentifierToken& token) noexcept
{
    if (token.infoPtr) {
        return LexerUtils::identifierCategoryToString(token.infoPtr->category);
    }
    return "UNKNOWN";
}

std::string TokenUtils::getIdentifierName(
    const IdentifierToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->name : "UNKNOWN";
}

std::string TokenUtils::getFullyQualifiedName(
    const IdentifierToken& token) noexcept
{
    if (!token.infoPtr) return "UNKNOWN";

    std::string result;
    if (!token.infoPtr->database.empty()) {
        result += token.infoPtr->database + ".";
    }
    if (!token.infoPtr->schema.empty()) {
        result += token.infoPtr->schema + ".";
    }
    result += token.infoPtr->name;
    return result;
}

// === Operator Token Utilities ===

std::string TokenUtils::getOperatorClassName(
    const OperatorToken& token) noexcept
{
    if (token.infoPtr) {
        return LexerUtils::operatorCategoryToString(token.infoPtr->category);
    }
    return "UNKNOWN";
}

std::string TokenUtils::getOperatorName(
    const OperatorToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->symbol : "UNKNOWN";
}

std::string TokenUtils::getOperatorSymbol(
    const OperatorToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->symbol : "UNKNOWN";
}

std::string TokenUtils::getOperatorInfo(
    const OperatorToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->toString() : "UNKNOWN";
}

// === Punctuator Token Utilities ===

std::string TokenUtils::getPunctuatorClassName(
    const PunctuatorToken& token) noexcept
{
    if (!token.infoPtr) return "UNKNOWN";

    if (token.infoPtr->commonSymbol != CommonSymbol::UNKNOWN) {
        return "CommonSymbol";
    }
    if (token.infoPtr->tsqlSymbol != TSQLSymbol::UNKNOWN) {
        return "TSQLSymbol";
    }
    if (token.infoPtr->stringDelimiter != StringDelimiter::UNKNOWN) {
        return "StringDelimiter";
    }
    return "UNKNOWN";
}

std::string TokenUtils::getPunctuatorName(
    const PunctuatorToken& token) noexcept
{
    if (!token.infoPtr) return "UNKNOWN";

    if (token.infoPtr->commonSymbol != CommonSymbol::UNKNOWN) {
        return LexerUtils::CommonSymbolTypeToString(token.infoPtr->commonSymbol);
    }
    if (token.infoPtr->tsqlSymbol != TSQLSymbol::UNKNOWN) {
        return LexerUtils::TSQLSymbolTypeToString(token.infoPtr->tsqlSymbol);
    }
    if (token.infoPtr->stringDelimiter != StringDelimiter::UNKNOWN) {
        return LexerUtils::StringDelimiterTypeToString(token.infoPtr->stringDelimiter);
    }
    return "UNKNOWN";
}

std::string TokenUtils::getPunctuatorSymbol(
    const PunctuatorToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->lexeme : "UNKNOWN";
}

std::string TokenUtils::getPunctuatorInfo(
    const PunctuatorToken& token) noexcept
{
    return token.infoPtr ? token.infoPtr->toString() : "UNKNOWN";
}