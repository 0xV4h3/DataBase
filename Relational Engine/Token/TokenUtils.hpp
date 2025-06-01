#pragma once
#include "Token.hpp"
#include "LexerUtils.hpp"

/**
 * @brief Utility class for extracting display strings from specialized token types.
 */
class TokenUtils {
public:
    // ---- KeywordToken ----
    static std::string getKeywordClassName(const KeywordToken& token) {
        if (token.infoPtr)
            return LexerUtils::keywordCategoryToString(token.infoPtr->category);
        return "UNKNOWN";
    }
    static std::string getKeywordName(const KeywordToken& token) {
        if (!token.infoPtr)
            return "UNKNOWN";
        switch (token.infoPtr->category) {
        case KeywordCategory::DML:
            return LexerUtils::DMLKeywordTypeToString(static_cast<DMLKeyword>(token.infoPtr->subKind));
        case KeywordCategory::DDL:
            return LexerUtils::DDLKeywordTypeToString(static_cast<DDLKeyword>(token.infoPtr->subKind));
        case KeywordCategory::CLAUSE:
            return LexerUtils::ClauseKeywordTypeToString(static_cast<ClauseKeyword>(token.infoPtr->subKind));
        case KeywordCategory::CTE:
            return LexerUtils::CTEKeywordTypeToString(static_cast<CTEKeyword>(token.infoPtr->subKind));
        case KeywordCategory::SETOP:
            return LexerUtils::SetOpKeywordTypeToString(static_cast<SetOpKeyword>(token.infoPtr->subKind));
        case KeywordCategory::WORD_OP:
            return LexerUtils::WordOperatorKeywordTypeToString(static_cast<WordOperatorKeyword>(token.infoPtr->subKind));
        case KeywordCategory::LOGICAL_CONST:
            return LexerUtils::LogicalConstantKeywordTypeToString(static_cast<LogicalConstantKeyword>(token.infoPtr->subKind));
        case KeywordCategory::TRANSACTION:
            return LexerUtils::TransactionKeywordTypeToString(static_cast<TransactionKeyword>(token.infoPtr->subKind));
        case KeywordCategory::SECURITY:
            return LexerUtils::SecurityKeywordTypeToString(static_cast<SecurityKeyword>(token.infoPtr->subKind));
        case KeywordCategory::PROG_STMT:
            return LexerUtils::ProgStmtKeywordTypeToString(static_cast<ProgStmtKeyword>(token.infoPtr->subKind));
        case KeywordCategory::MISC:
            return LexerUtils::MiscKeywordTypeToString(static_cast<MiscKeyword>(token.infoPtr->subKind));
        default:
            return "UNKNOWN";
        }
    }

    // ---- FunctionToken ----
    static std::string getFunctionClassName(const FunctionToken& token) {
        if (token.infoPtr)
            return LexerUtils::FunctionCategoryTypeToString(token.infoPtr->category);
        return "UNKNOWN";
    }
    static std::string getFunctionName(const FunctionToken& token) {
        if (token.infoPtr)
            return token.infoPtr->name;
        return "UNKNOWN";
    }

    // ---- OperatorToken ----
    static std::string getOperatorClassName(const OperatorToken& token) {
        if (token.infoPtr)
            return LexerUtils::operatorCategoryToString(token.infoPtr->category);
        return "UNKNOWN";
    }
    static std::string getOperatorName(const OperatorToken& token) {
        if (token.infoPtr)
            return token.infoPtr->symbol;
        return "UNKNOWN";
    }

    // ---- PunctuatorToken ----
    static std::string getPunctuatorClassName(const PunctuatorToken& token) {
        if (token.infoPtr)
            return LexerUtils::punctuatorCategoryToString(token.infoPtr->category);
        return "UNKNOWN";
    }
    static std::string getPunctuatorName(const PunctuatorToken& token) {
        if (!token.infoPtr)
            return "UNKNOWN";
        switch (token.infoPtr->category) {
        case PunctuatorCategory::COMMON:
            return LexerUtils::CommonSymbolTypeToString(static_cast<CommonSymbol>(token.infoPtr->subKind));
        case PunctuatorCategory::TSQL:
            return LexerUtils::TSQLSymbolTypeToString(static_cast<TSQLSymbol>(token.infoPtr->subKind));
        case PunctuatorCategory::STRING_DELIM:
            return LexerUtils::StringDelimiterTypeToString(static_cast<StringDelimiter>(token.infoPtr->subKind));
        default:
            return "UNKNOWN";
        }
    }
};