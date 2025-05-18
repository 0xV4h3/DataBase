#pragma once
#include <string>
#include "Token.hpp"

/**
 * @brief Utility class for converting all lexer token enums to string representations.
 *
 * All methods are static and provide readable string values for token classification.
 *
 * NOTE: This file contains all implementations inline for simplicity and performance.
 */
class LexerUtils {
public:
    // -------------------------------------------------
    // TokenType enum to string
    // -------------------------------------------------
    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::KEYWORD:      return "KEYWORD";
        case TokenType::FUNCTION:     return "FUNCTION";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::LITERAL:      return "LITERAL";
        case TokenType::OPERATOR:     return "OPERATOR";
        case TokenType::PUNCTUATOR:   return "PUNCTUATOR";
        case TokenType::DATETIMEPART: return "DATETIMEPART";
        case TokenType::COMMENT:      return "COMMENT";
        case TokenType::END_OF_FILE:  return "END_OF_FILE";
        case TokenType::UNKNOWN:      return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // KeywordCategory enum to string
    // -------------------------------------------------
    static std::string keywordCategoryToString(KeywordCategory cat) {
        switch (cat) {
        case KeywordCategory::DML:           return "DMLKeyword";
        case KeywordCategory::DDL:           return "DDLKeyword";
        case KeywordCategory::CLAUSE:        return "ClauseKeyword";
        case KeywordCategory::CTE:           return "CTEKeyword";
        case KeywordCategory::SETOP:         return "SetOpKeyword";
        case KeywordCategory::WORD_OP:       return "WordOperatorKeyword";
        case KeywordCategory::LOGICAL_CONST: return "LogicalConstantKeyword";
        case KeywordCategory::TRANSACTION:   return "TransactionKeyword";
        case KeywordCategory::SECURITY:      return "SecurityKeyword";
        case KeywordCategory::PROG_STMT:     return "ProgStmtKeyword";
        case KeywordCategory::MISC:          return "MiscKeyword";
        default:                             return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // OperatorCategory enum to string
    // -------------------------------------------------
    static std::string operatorCategoryToString(OperatorCategory cat) {
        switch (cat) {
        case OperatorCategory::ARITHMETIC:   return "ArithmeticOp";
        case OperatorCategory::ASSIGN:       return "AssignOp";
        case OperatorCategory::COMPARISON:   return "ComparisonOp";
        case OperatorCategory::LOGICAL:      return "LogicalOp";
        case OperatorCategory::BITWISE:      return "BitwiseOp";
        case OperatorCategory::CONCAT:       return "ConcatOp";
        default:                             return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // PunctuatorCategory enum to string
    // -------------------------------------------------
    static std::string punctuatorCategoryToString(PunctuatorCategory cat) {
        switch (cat) {
        case PunctuatorCategory::COMMON:       return "CommonSymbol";
        case PunctuatorCategory::TSQL:         return "TSQLSymbol";
        case PunctuatorCategory::STRING_DELIM: return "StringDelimiter";
        default:                               return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // LiteralCategory enum to string
    // -------------------------------------------------
    static std::string literalTypeToString(LiteralCategory type) {
        switch (type) {
        case LiteralCategory::STRING:       return "STRING";
        case LiteralCategory::CHAR:         return "CHAR";
        case LiteralCategory::INTEGER:      return "INTEGER";
        case LiteralCategory::FLOAT:        return "FLOAT";
        case LiteralCategory::BINARY:       return "BINARY";
        case LiteralCategory::HEX:          return "HEX";
        case LiteralCategory::DATE:         return "DATE";
        case LiteralCategory::TIME:         return "TIME";
        case LiteralCategory::DATETIME:     return "DATETIME";
        case LiteralCategory::JSON:         return "JSON";
        case LiteralCategory::BOOLEAN:      return "BOOLEAN";
        case LiteralCategory::NULL_VALUE:   return "NULL";
        case LiteralCategory::UNKNOWN:      return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // IdentifierCategory enum to string
    // -------------------------------------------------
    static std::string identifierTypeToString(IdentifierCategory type) {
        switch (type) {
        case IdentifierCategory::TABLE:               return "TABLE";
        case IdentifierCategory::VIEW:                return "VIEW";
        case IdentifierCategory::PROCEDURE:           return "PROCEDURE";
        case IdentifierCategory::FUNCTION:            return "FUNCTION";
        case IdentifierCategory::TRIGGER:             return "TRIGGER";
        case IdentifierCategory::INDEX:               return "INDEX";
        case IdentifierCategory::CONSTRAINT:          return "CONSTRAINT";
        case IdentifierCategory::SCHEMA:              return "SCHEMA";
        case IdentifierCategory::DATABASE:            return "DATABASE";
        case IdentifierCategory::SEQUENCE:            return "SEQUENCE";
        case IdentifierCategory::USER_DEFINED_TYPE:   return "USER_DEFINED_TYPE";
        case IdentifierCategory::ROLE:                return "ROLE";
        case IdentifierCategory::USER:                return "USER";
        case IdentifierCategory::EXTERNAL_TABLE:      return "EXTERNAL_TABLE";
        case IdentifierCategory::USER_VARIABLE:       return "USER_VARIABLE";
        case IdentifierCategory::SYSTEM_VARIABLE:     return "SYSTEM_VARIABLE";
        case IdentifierCategory::TEMP_TABLE:          return "TEMP_TABLE";
        case IdentifierCategory::GLOBAL_TEMP_TABLE:   return "GLOBAL_TEMP_TABLE";
        case IdentifierCategory::UNKNOWN:             return "UNKNOWN";
        default:                                     return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // DMLKeyword enum to string
    // -------------------------------------------------
    static std::string DMLKeywordTypeToString(DMLKeyword kw) {
        switch (kw) {
        case DMLKeyword::SELECT:   return "SELECT";
        case DMLKeyword::INSERT:   return "INSERT";
        case DMLKeyword::UPDATE:   return "UPDATE";
        case DMLKeyword::DELETE:   return "DELETE";
        case DMLKeyword::MERGE:    return "MERGE";
        case DMLKeyword::EXECUTE:  return "EXECUTE";
        case DMLKeyword::UNKNOWN:  return "UNKNOWN";
        default:                   return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // DDLKeyword enum to string
    // -------------------------------------------------
    static std::string DDLKeywordTypeToString(DDLKeyword kw) {
        switch (kw) {
        case DDLKeyword::CREATE:   return "CREATE";
        case DDLKeyword::ALTER:    return "ALTER";
        case DDLKeyword::DROP:     return "DROP";
        case DDLKeyword::TRUNCATE: return "TRUNCATE";
        case DDLKeyword::UNKNOWN:  return "UNKNOWN";
        default:                   return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ClauseKeyword enum to string
    // -------------------------------------------------
    static std::string ClauseKeywordTypeToString(ClauseKeyword kw) {
        switch (kw) {
        case ClauseKeyword::FROM:     return "FROM";
        case ClauseKeyword::WHERE:    return "WHERE";
        case ClauseKeyword::JOIN:     return "JOIN";
        case ClauseKeyword::ON:       return "ON";
        case ClauseKeyword::GROUP:    return "GROUP";
        case ClauseKeyword::BY:       return "BY";
        case ClauseKeyword::HAVING:   return "HAVING";
        case ClauseKeyword::ORDER:    return "ORDER";
        case ClauseKeyword::UNION:    return "UNION";
        case ClauseKeyword::ALL:      return "ALL";
        case ClauseKeyword::DISTINCT: return "DISTINCT";
        case ClauseKeyword::TOP:      return "TOP";
        case ClauseKeyword::LIMIT:    return "LIMIT";
        case ClauseKeyword::OFFSET:   return "OFFSET";
        case ClauseKeyword::LEFT:     return "LEFT";
        case ClauseKeyword::RIGHT:    return "RIGHT";
        case ClauseKeyword::FULL:     return "FULL";
        case ClauseKeyword::OUTER:    return "OUTER";
        case ClauseKeyword::INNER:    return "INNER";
        case ClauseKeyword::CROSS:    return "CROSS";
        case ClauseKeyword::APPLY:    return "APPLY";
        case ClauseKeyword::UNKNOWN:  return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // CTEKeyword enum to string
    // -------------------------------------------------
    static std::string CTEKeywordTypeToString(CTEKeyword kw) {
        switch (kw) {
        case CTEKeyword::WITH:    return "WITH";
        case CTEKeyword::UNKNOWN: return "UNKNOWN";
        default:                  return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // SetOpKeyword enum to string
    // -------------------------------------------------
    static std::string SetOpKeywordTypeToString(SetOpKeyword kw) {
        switch (kw) {
        case SetOpKeyword::EXCEPT:     return "EXCEPT";
        case SetOpKeyword::INTERSECT:  return "INTERSECT";
        case SetOpKeyword::UNKNOWN:    return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // WordOperatorKeyword enum to string
    // -------------------------------------------------
    static std::string WordOperatorKeywordTypeToString(WordOperatorKeyword kw) {
        switch (kw) {
        case WordOperatorKeyword::IN:      return "IN";
        case WordOperatorKeyword::IS:      return "IS";
        case WordOperatorKeyword::NOT:     return "NOT";
        case WordOperatorKeyword::LIKE:    return "LIKE";
        case WordOperatorKeyword::BETWEEN: return "BETWEEN";
        case WordOperatorKeyword::UNKNOWN: return "UNKNOWN";
        default:                           return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // LogicalConstantKeyword enum to string
    // -------------------------------------------------
    static std::string LogicalConstantKeywordTypeToString(LogicalConstantKeyword kw) {
        switch (kw) {
        case LogicalConstantKeyword::NULL_KEYWORD:  return "NULL";
        case LogicalConstantKeyword::TRUE_KEYWORD:  return "TRUE";
        case LogicalConstantKeyword::FALSE_KEYWORD: return "FALSE";
        case LogicalConstantKeyword::UNKNOWN:       return "UNKNOWN";
        default:                                    return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // TransactionKeyword enum to string
    // -------------------------------------------------
    static std::string TransactionKeywordTypeToString(TransactionKeyword kw) {
        switch (kw) {
        case TransactionKeyword::BEGIN:     return "BEGIN";
        case TransactionKeyword::COMMIT:    return "COMMIT";
        case TransactionKeyword::ROLLBACK:  return "ROLLBACK";
        case TransactionKeyword::SAVEPOINT: return "SAVEPOINT";
        case TransactionKeyword::UNKNOWN:   return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // SecurityKeyword enum to string
    // -------------------------------------------------
    static std::string SecurityKeywordTypeToString(SecurityKeyword kw) {
        switch (kw) {
        case SecurityKeyword::GRANT:   return "GRANT";
        case SecurityKeyword::REVOKE:  return "REVOKE";
        case SecurityKeyword::DENY:    return "DENY";
        case SecurityKeyword::UNKNOWN: return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ProgStmtKeyword enum to string
    // -------------------------------------------------
    static std::string ProgStmtKeywordTypeToString(ProgStmtKeyword kw) {
        switch (kw) {
        case ProgStmtKeyword::DECLARE: return "DECLARE";
        case ProgStmtKeyword::SET:     return "SET";
        case ProgStmtKeyword::PRINT:   return "PRINT";
        case ProgStmtKeyword::RETURN:  return "RETURN";
        case ProgStmtKeyword::THROW:   return "THROW";
        case ProgStmtKeyword::TRY:     return "TRY";
        case ProgStmtKeyword::CATCH:   return "CATCH";
        case ProgStmtKeyword::UNKNOWN: return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // MiscKeyword enum to string
    // -------------------------------------------------
    static std::string MiscKeywordTypeToString(MiscKeyword kw) {
        switch (kw) {
        case MiscKeyword::AS:      return "AS";
        case MiscKeyword::CASE:    return "CASE";
        case MiscKeyword::WHEN:    return "WHEN";
        case MiscKeyword::THEN:    return "THEN";
        case MiscKeyword::ELSE:    return "ELSE";
        case MiscKeyword::END:     return "END";
        case MiscKeyword::CAST:    return "CAST";
        case MiscKeyword::CONVERT: return "CONVERT";
        case MiscKeyword::ASC:     return "ASC";
        case MiscKeyword::DESC:    return "DESC";
        case MiscKeyword::UNKNOWN: return "UNKNOWN";
        default:                   return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // FunctionCategory enum to string
    // -------------------------------------------------
    static std::string FunctionCategoryTypeToString(FunctionCategory cat) {
        switch (cat) {
        case FunctionCategory::AGGREGATE:     return "AGGREGATE";
        case FunctionCategory::SCALAR:        return "SCALAR";
        case FunctionCategory::STRING:        return "STRING";
        case FunctionCategory::DATETIME:      return "DATETIME";
        case FunctionCategory::MATHEMATICAL:  return "MATHEMATICAL";
        case FunctionCategory::SYSTEM:        return "SYSTEM";
        case FunctionCategory::UNKNOWN:       return "UNKNOWN";
        default:                              return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // AggregateFunction enum to string
    // -------------------------------------------------
    static std::string AggregateFunctionTypeToString(AggregateFunction fn) {
        switch (fn) {
        case AggregateFunction::COUNT:  return "COUNT";
        case AggregateFunction::SUM:    return "SUM";
        case AggregateFunction::AVG:    return "AVG";
        case AggregateFunction::MIN:    return "MIN";
        case AggregateFunction::MAX:    return "MAX";
        case AggregateFunction::UNKNOWN:return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ScalarFunction enum to string
    // -------------------------------------------------
    static std::string ScalarFunctionTypeToString(ScalarFunction fn) {
        switch (fn) {
        case ScalarFunction::CONVERT:   return "CONVERT";
        case ScalarFunction::CAST:      return "CAST";
        case ScalarFunction::COALESCE:  return "COALESCE";
        case ScalarFunction::LENGTH:    return "LENGTH";
        case ScalarFunction::UNKNOWN:   return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // StringFunction enum to string
    // -------------------------------------------------
    static std::string StringFunctionTypeToString(StringFunction fn) {
        switch (fn) {
        case StringFunction::UPPER:     return "UPPER";
        case StringFunction::LOWER:     return "LOWER";
        case StringFunction::SUBSTRING: return "SUBSTRING";
        case StringFunction::TRIM:      return "TRIM";
        case StringFunction::UNKNOWN:   return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // DateTimeFunction enum to string
    // -------------------------------------------------
    static std::string DateTimeFunctionTypeToString(DateTimeFunction fn) {
        switch (fn) {
        case DateTimeFunction::DATEPART:  return "DATEPART";
        case DateTimeFunction::GETDATE:   return "GETDATE";
        case DateTimeFunction::NOW:       return "NOW";
        case DateTimeFunction::UNKNOWN:   return "UNKNOWN";
        default:                          return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // MathFunction enum to string
    // -------------------------------------------------
    static std::string MathFunctionTypeToString(MathFunction fn) {
        switch (fn) {
        case MathFunction::ABS:      return "ABS";
        case MathFunction::CEILING:  return "CEILING";
        case MathFunction::FLOOR:    return "FLOOR";
        case MathFunction::ROUND:    return "ROUND";
        case MathFunction::UNKNOWN:  return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // SystemFunction enum to string
    // -------------------------------------------------
    static std::string SystemFunctionTypeToString(SystemFunction fn) {
        switch (fn) {
        case SystemFunction::SUSER_SNAME:   return "SUSER_SNAME";
        case SystemFunction::CURRENT_USER:  return "CURRENT_USER";
        case SystemFunction::TRANCOUNT:     return "TRANCOUNT";
        case SystemFunction::UNKNOWN:       return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ArithmeticOp enum to string
    // -------------------------------------------------
    static std::string ArithmeticOpTypeToString(ArithmeticOp op) {
        switch (op) {
        case ArithmeticOp::PLUS:     return "PLUS";
        case ArithmeticOp::MINUS:    return "MINUS";
        case ArithmeticOp::MULTIPLY: return "MULTIPLY";
        case ArithmeticOp::DIVIDE:   return "DIVIDE";
        case ArithmeticOp::MOD:      return "MOD";
        case ArithmeticOp::UNKNOWN:  return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // AssignOp enum to string
    // -------------------------------------------------
    static std::string AssignOpTypeToString(AssignOp op) {
        switch (op) {
        case AssignOp::ASSIGN:   return "ASSIGN";
        case AssignOp::UNKNOWN:  return "UNKNOWN";
        default:                 return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ComparisonOp enum to string
    // -------------------------------------------------
    static std::string ComparisonOpTypeToString(ComparisonOp op) {
        switch (op) {
        case ComparisonOp::LESS:         return "LESS";
        case ComparisonOp::GREATER:      return "GREATER";
        case ComparisonOp::LESS_EQUAL:   return "LESS_EQUAL";
        case ComparisonOp::GREATER_EQUAL:return "GREATER_EQUAL";
        case ComparisonOp::NOT_EQUAL:    return "NOT_EQUAL";
        case ComparisonOp::EQUAL:        return "EQUAL";
        case ComparisonOp::UNKNOWN:      return "UNKNOWN";
        default:                         return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // LogicalOp enum to string
    // -------------------------------------------------
    static std::string LogicalOpTypeToString(LogicalOp op) {
        switch (op) {
        case LogicalOp::AND:     return "AND";
        case LogicalOp::OR:      return "OR";
        case LogicalOp::NOT:     return "NOT";
        case LogicalOp::UNKNOWN: return "UNKNOWN";
        default:                 return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // BitwiseOp enum to string
    // -------------------------------------------------
    static std::string BitwiseOpTypeToString(BitwiseOp op) {
        switch (op) {
        case BitwiseOp::BITWISE_AND:   return "BITWISE_AND";
        case BitwiseOp::BITWISE_OR:    return "BITWISE_OR";
        case BitwiseOp::BITWISE_XOR:   return "BITWISE_XOR";
        case BitwiseOp::BITWISE_NOT:   return "BITWISE_NOT";
        case BitwiseOp::LEFT_SHIFT:    return "LEFT_SHIFT";
        case BitwiseOp::RIGHT_SHIFT:   return "RIGHT_SHIFT";
        case BitwiseOp::UNKNOWN:       return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // ConcatOp enum to string
    // -------------------------------------------------
    static std::string ConcatOpTypeToString(ConcatOp op) {
        switch (op) {
        case ConcatOp::CONCAT:   return "CONCAT";
        case ConcatOp::UNKNOWN:  return "UNKNOWN";
        default:                 return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // DateTimePart enum to string
    // -------------------------------------------------
    static std::string DateTimePartTypeToString(DateTimePart part) {
        switch (part) {
        case DateTimePart::YEAR:            return "YEAR";
        case DateTimePart::QUARTER:         return "QUARTER";
        case DateTimePart::MONTH:           return "MONTH";
        case DateTimePart::DAY_OF_YEAR:     return "DAY_OF_YEAR";
        case DateTimePart::DAY:             return "DAY";
        case DateTimePart::WEEK:            return "WEEK";
        case DateTimePart::ISO_WEEK:        return "ISO_WEEK";
        case DateTimePart::WEEKDAY:         return "WEEKDAY";
        case DateTimePart::HOUR:            return "HOUR";
        case DateTimePart::MINUTE:          return "MINUTE";
        case DateTimePart::SECOND:          return "SECOND";
        case DateTimePart::MILLISECOND:     return "MILLISECOND";
        case DateTimePart::MICROSECOND:     return "MICROSECOND";
        case DateTimePart::NANOSECOND:      return "NANOSECOND";
        case DateTimePart::TIMEZONE_OFFSET: return "TIMEZONE_OFFSET";
        case DateTimePart::UNKNOWN:         return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // CommonSymbol enum to string
    // -------------------------------------------------
    static std::string CommonSymbolTypeToString(CommonSymbol sym) {
        switch (sym) {
        case CommonSymbol::COMMA:      return "COMMA";
        case CommonSymbol::SEMICOLON:  return "SEMICOLON";
        case CommonSymbol::LPAREN:     return "LPAREN";
        case CommonSymbol::RPAREN:     return "RPAREN";
        case CommonSymbol::LBRACE:     return "LBRACE";
        case CommonSymbol::RBRACE:     return "RBRACE";
        case CommonSymbol::LBRACKET:   return "LBRACKET";
        case CommonSymbol::RBRACKET:   return "RBRACKET";
        case CommonSymbol::UNKNOWN:    return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // TSQLSymbol enum to string
    // -------------------------------------------------
    static std::string TSQLSymbolTypeToString(TSQLSymbol sym) {
        switch (sym) {
        case TSQLSymbol::DOT:         return "DOT";
        case TSQLSymbol::COLON:       return "COLON";
        case TSQLSymbol::DOUBLE_COLON:return "DOUBLE_COLON";
        case TSQLSymbol::UNKNOWN:     return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // StringDelimiter enum to string
    // -------------------------------------------------
    static std::string StringDelimiterTypeToString(StringDelimiter delim) {
        switch (delim) {
        case StringDelimiter::SINGLE_QUOTE: return "SINGLE_QUOTE";
        case StringDelimiter::DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        case StringDelimiter::BACKTICK:     return "BACKTICK";
        case StringDelimiter::UNKNOWN:      return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    // -------------------------------------------------
    // CommentType enum to string
    // -------------------------------------------------
    static std::string commentTypeToString(CommentType ct) {
        switch (ct) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE:  return "MULTI_LINE";
        default:                       return "UNKNOWN";
        }
    }
};