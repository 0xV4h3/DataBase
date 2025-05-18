#pragma once
#include "Token.hpp"
#include "LexerUtils.hpp"

/**
 * @brief Utility class for extracting display strings from specialized Token types.
 *
 * All methods are static and return a human-readable string for a given token.
 * No logic is changed; only style and clarity are improved.
 */
class TokenUtils {
public:
    // ==========================
    // KeywordToken utilities
    // ==========================

    /**
     * @brief Returns the class/category string for a KeywordToken.
     * @param token The KeywordToken.
     * @return String describing the keyword category.
     */
    static std::string getKeywordClassName(const KeywordToken& token) {
        return LexerUtils::keywordCategoryToString(token.category);
    }

    /**
     * @brief Returns the exact keyword string for a KeywordToken.
     * @param token The KeywordToken.
     * @return String describing the specific keyword.
     */
    static std::string getKeywordName(const KeywordToken& token) {
        switch (token.category) {
        case KeywordCategory::DML:
            return LexerUtils::DMLKeywordTypeToString(token.kind.dml);
        case KeywordCategory::DDL:
            return LexerUtils::DDLKeywordTypeToString(token.kind.ddl);
        case KeywordCategory::CLAUSE:
            return LexerUtils::ClauseKeywordTypeToString(token.kind.clause);
        case KeywordCategory::CTE:
            return LexerUtils::CTEKeywordTypeToString(token.kind.cte);
        case KeywordCategory::SETOP:
            return LexerUtils::SetOpKeywordTypeToString(token.kind.setop);
        case KeywordCategory::WORD_OP:
            return LexerUtils::WordOperatorKeywordTypeToString(token.kind.wordop);
        case KeywordCategory::LOGICAL_CONST:
            return LexerUtils::LogicalConstantKeywordTypeToString(token.kind.lconst);
        case KeywordCategory::TRANSACTION:
            return LexerUtils::TransactionKeywordTypeToString(token.kind.tran);
        case KeywordCategory::SECURITY:
            return LexerUtils::SecurityKeywordTypeToString(token.kind.sec);
        case KeywordCategory::PROG_STMT:
            return LexerUtils::ProgStmtKeywordTypeToString(token.kind.prog);
        case KeywordCategory::MISC:
            return LexerUtils::MiscKeywordTypeToString(token.kind.misc);
        default:
            return "UNKNOWN";
        }
    }

    // ==========================
    // FunctionToken utilities
    // ==========================

    /**
     * @brief Returns the class/category string for a FunctionToken.
     * @param token The FunctionToken.
     * @return String describing the function category.
     */
    static std::string getFunctionClassName(const FunctionToken& token) {
        return LexerUtils::FunctionCategoryTypeToString(token.category);
    }

    /**
     * @brief Returns the exact function name string for a FunctionToken.
     * @param token The FunctionToken.
     * @return String describing the specific function.
     */
    static std::string getFunctionName(const FunctionToken& token) {
        switch (token.category) {
        case FunctionCategory::AGGREGATE:
            return LexerUtils::AggregateFunctionTypeToString(token.func.agg);
        case FunctionCategory::SCALAR:
            return LexerUtils::ScalarFunctionTypeToString(token.func.scal);
        case FunctionCategory::STRING:
            return LexerUtils::StringFunctionTypeToString(token.func.str);
        case FunctionCategory::DATETIME:
            return LexerUtils::DateTimeFunctionTypeToString(token.func.dt);
        case FunctionCategory::MATHEMATICAL:
            return LexerUtils::MathFunctionTypeToString(token.func.math);
        case FunctionCategory::SYSTEM:
            return LexerUtils::SystemFunctionTypeToString(token.func.sys);
        default:
            return "UNKNOWN";
        }
    }

    // ==========================
    // OperatorToken utilities
    // ==========================

    /**
     * @brief Returns the class/category string for an OperatorToken.
     * @param token The OperatorToken.
     * @return String describing the operator category.
     */
    static std::string getOperatorClassName(const OperatorToken& token) {
        return LexerUtils::operatorCategoryToString(token.category);
    }

    /**
     * @brief Returns the operator name string for an OperatorToken.
     * @param token The OperatorToken.
     * @return String describing the specific operator.
     */
    static std::string getOperatorName(const OperatorToken& token) {
        switch (token.category) {
        case OperatorCategory::ARITHMETIC:
            return LexerUtils::ArithmeticOpTypeToString(token.op.ar);
        case OperatorCategory::ASSIGN:
            return LexerUtils::AssignOpTypeToString(token.op.as);
        case OperatorCategory::COMPARISON:
            return LexerUtils::ComparisonOpTypeToString(token.op.comp);
        case OperatorCategory::LOGICAL:
            return LexerUtils::LogicalOpTypeToString(token.op.log);
        case OperatorCategory::BITWISE:
            return LexerUtils::BitwiseOpTypeToString(token.op.bit);
        case OperatorCategory::CONCAT:
            return LexerUtils::ConcatOpTypeToString(token.op.concat);
        default:
            return "UNKNOWN";
        }
    }

    // ==========================
    // PunctuatorToken utilities
    // ==========================

    /**
     * @brief Returns the class/category string for a PunctuatorToken.
     * @param token The PunctuatorToken.
     * @return String describing the punctuator category.
     */
    static std::string getPunctuatorClassName(const PunctuatorToken& token) {
        return LexerUtils::punctuatorCategoryToString(token.category);
    }

    /**
     * @brief Returns the punctuator name string for a PunctuatorToken.
     * @param token The PunctuatorToken.
     * @return String describing the specific punctuator.
     */
    static std::string getPunctuatorName(const PunctuatorToken& token) {
        switch (token.category) {
        case PunctuatorCategory::COMMON:
            return LexerUtils::CommonSymbolTypeToString(token.sym.cs);
        case PunctuatorCategory::TSQL:
            return LexerUtils::TSQLSymbolTypeToString(token.sym.ts);
        case PunctuatorCategory::STRING_DELIM:
            return LexerUtils::StringDelimiterTypeToString(token.sym.sd);
        default:
            return "UNKNOWN";
        }
    }
};