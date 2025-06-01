#pragma once
#include <string>
#include "TokenEnums.hpp"

/**
 * @brief Utility class for converting lexer token enums to string representations.
 */
class LexerUtils {
public:
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
        case KeywordCategory::UNKNOWN:       return "UNKNOWN";
        default:                             return "UNKNOWN";
        }
    }

    static std::string operatorCategoryToString(OperatorCategory cat) {
        switch (cat) {
        case OperatorCategory::ARITHMETIC:   return "ArithmeticOp";
        case OperatorCategory::ASSIGN:       return "AssignOp";
        case OperatorCategory::COMPARISON:   return "ComparisonOp";
        case OperatorCategory::LOGICAL:      return "LogicalOp";
        case OperatorCategory::BITWISE:      return "BitwiseOp";
        case OperatorCategory::CONCAT:       return "ConcatOp";
        case OperatorCategory::JSON:         return "JsonOp";
        case OperatorCategory::REGEX:        return "RegexOp";
        case OperatorCategory::ARRAY:        return "ArrayOp";
        case OperatorCategory::TYPECAST:     return "TypecastOp";
        case OperatorCategory::UNKNOWN:      return "UNKNOWN";
        default:                             return "UNKNOWN";
        }
    }

    static std::string punctuatorCategoryToString(PunctuatorCategory cat) {
        switch (cat) {
        case PunctuatorCategory::COMMON:       return "CommonSymbol";
        case PunctuatorCategory::TSQL:         return "TSQLSymbol";
        case PunctuatorCategory::STRING_DELIM: return "StringDelimiter";
        case PunctuatorCategory::DOLLAR_QUOTE: return "DollarQuote";
        case PunctuatorCategory::PARAM_MARKER: return "ParamMarker";
        case PunctuatorCategory::UNKNOWN:      return "UNKNOWN";
        default:                               return "UNKNOWN";
        }
    }

    static std::string literalTypeToString(LiteralCategory type) {
        switch (type) {
        case LiteralCategory::STRING:         return "STRING";
        case LiteralCategory::ESCAPE_STRING:  return "ESCAPE_STRING";
        case LiteralCategory::CHAR:           return "CHAR";
        case LiteralCategory::INTEGER:        return "INTEGER";
        case LiteralCategory::FLOAT:          return "FLOAT";
        case LiteralCategory::BINARY:         return "BINARY";
        case LiteralCategory::HEX:            return "HEX";
        case LiteralCategory::DATE:           return "DATE";
        case LiteralCategory::TIME:           return "TIME";
        case LiteralCategory::DATETIME:       return "DATETIME";
        case LiteralCategory::INTERVAL:       return "INTERVAL";
        case LiteralCategory::UUID:           return "UUID";
        case LiteralCategory::ARRAY:          return "ARRAY";
        case LiteralCategory::JSON:           return "JSON";
        case LiteralCategory::XML:            return "XML";
        case LiteralCategory::BOOLEAN:        return "BOOLEAN";
        case LiteralCategory::NULL_VALUE:     return "NULL";
        case LiteralCategory::UNKNOWN:        return "UNKNOWN";
        default:                              return "UNKNOWN";
        }
    }

    static std::string identifierTypeToString(IdentifierCategory type) {
        switch (type) {
        case IdentifierCategory::TABLE:             return "TABLE";
        case IdentifierCategory::VIEW:              return "VIEW";
        case IdentifierCategory::PROCEDURE:         return "PROCEDURE";
        case IdentifierCategory::FUNCTION:          return "FUNCTION";
        case IdentifierCategory::TRIGGER:           return "TRIGGER";
        case IdentifierCategory::INDEX:             return "INDEX";
        case IdentifierCategory::CONSTRAINT:        return "CONSTRAINT";
        case IdentifierCategory::SCHEMA:            return "SCHEMA";
        case IdentifierCategory::DATABASE:          return "DATABASE";
        case IdentifierCategory::SEQUENCE:          return "SEQUENCE";
        case IdentifierCategory::USER_DEFINED_TYPE: return "USER_DEFINED_TYPE";
        case IdentifierCategory::ROLE:              return "ROLE";
        case IdentifierCategory::USER:              return "USER";
        case IdentifierCategory::EXTERNAL_TABLE:    return "EXTERNAL_TABLE";
        case IdentifierCategory::USER_VARIABLE:     return "USER_VARIABLE";
        case IdentifierCategory::SYSTEM_VARIABLE:   return "SYSTEM_VARIABLE";
        case IdentifierCategory::TEMP_TABLE:        return "TEMP_TABLE";
        case IdentifierCategory::GLOBAL_TEMP_TABLE: return "GLOBAL_TEMP_TABLE";
        case IdentifierCategory::COLUMN:            return "COLUMN";
        case IdentifierCategory::PARAMETER:         return "PARAMETER";
        case IdentifierCategory::LABEL:             return "LABEL";
        case IdentifierCategory::UNKNOWN:           return "UNKNOWN";
        default:                                   return "UNKNOWN";
        }
    }

    static std::string DMLKeywordTypeToString(DMLKeyword kw) {
        switch (kw) {
        case DMLKeyword::SELECT:     return "SELECT";
        case DMLKeyword::INSERT:     return "INSERT";
        case DMLKeyword::UPDATE:     return "UPDATE";
        case DMLKeyword::DELETE:     return "DELETE";
        case DMLKeyword::MERGE:      return "MERGE";
        case DMLKeyword::EXECUTE:    return "EXECUTE";
        case DMLKeyword::VALUES:     return "VALUES";
        case DMLKeyword::OUTPUT:     return "OUTPUT";
        case DMLKeyword::DEFAULT:    return "DEFAULT";
        case DMLKeyword::INTO:       return "INTO";
        case DMLKeyword::RETURNING:  return "RETURNING";
        case DMLKeyword::USING:      return "USING";
        case DMLKeyword::UNKNOWN:    return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

    static std::string DDLKeywordTypeToString(DDLKeyword kw) {
        switch (kw) {
        case DDLKeyword::CREATE:     return "CREATE";
        case DDLKeyword::ALTER:      return "ALTER";
        case DDLKeyword::DROP:       return "DROP";
        case DDLKeyword::TRUNCATE:   return "TRUNCATE";
        case DDLKeyword::TABLE:      return "TABLE";
        case DDLKeyword::VIEW:       return "VIEW";
        case DDLKeyword::INDEX:      return "INDEX";
        case DDLKeyword::SEQUENCE:   return "SEQUENCE";
        case DDLKeyword::CONSTRAINT: return "CONSTRAINT";
        case DDLKeyword::TRIGGER:    return "TRIGGER";
        case DDLKeyword::PRIMARY:    return "PRIMARY";
        case DDLKeyword::FOREIGN:    return "FOREIGN";
        case DDLKeyword::REFERENCES: return "REFERENCES";
        case DDLKeyword::UNIQUE:     return "UNIQUE";
        case DDLKeyword::CHECK:      return "CHECK";
        case DDLKeyword::PARTITION:  return "PARTITION";
        case DDLKeyword::COLUMN:     return "COLUMN";
        case DDLKeyword::DATABASE:   return "DATABASE";
        case DDLKeyword::SCHEMA:     return "SCHEMA";
        case DDLKeyword::TYPE:       return "TYPE";
        case DDLKeyword::UNKNOWN:    return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

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
        case ClauseKeyword::WINDOW:   return "WINDOW";
        case ClauseKeyword::PARTITION:return "PARTITION";
        case ClauseKeyword::OVER:     return "OVER";
        case ClauseKeyword::AS:       return "AS";
        case ClauseKeyword::USING:    return "USING";
        case ClauseKeyword::DO:       return "DO";
        case ClauseKeyword::END:      return "END";
        case ClauseKeyword::UNKNOWN:  return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    static std::string CTEKeywordTypeToString(CTEKeyword kw) {
        switch (kw) {
        case CTEKeyword::WITH:      return "WITH";
        case CTEKeyword::RECURSIVE: return "RECURSIVE";
        case CTEKeyword::UNKNOWN:   return "UNKNOWN";
        default:                    return "UNKNOWN";
        }
    }

    static std::string SetOpKeywordTypeToString(SetOpKeyword kw) {
        switch (kw) {
        case SetOpKeyword::EXCEPT:    return "EXCEPT";
        case SetOpKeyword::INTERSECT: return "INTERSECT";
        case SetOpKeyword::UNION:     return "UNION";
        case SetOpKeyword::UNKNOWN:   return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    static std::string WordOperatorKeywordTypeToString(WordOperatorKeyword kw) {
        switch (kw) {
        case WordOperatorKeyword::IN:      return "IN";
        case WordOperatorKeyword::IS:      return "IS";
        case WordOperatorKeyword::NOT:     return "NOT";
        case WordOperatorKeyword::LIKE:    return "LIKE";
        case WordOperatorKeyword::BETWEEN: return "BETWEEN";
        case WordOperatorKeyword::EXISTS:  return "EXISTS";
        case WordOperatorKeyword::ALL:     return "ALL";
        case WordOperatorKeyword::ANY:     return "ANY";
        case WordOperatorKeyword::SOME:    return "SOME";
        case WordOperatorKeyword::UNIQUE:  return "UNIQUE";
        case WordOperatorKeyword::UNKNOWN: return "UNKNOWN";
        default:                           return "UNKNOWN";
        }
    }

    static std::string LogicalConstantKeywordTypeToString(LogicalConstantKeyword kw) {
        switch (kw) {
        case LogicalConstantKeyword::NULL_KEYWORD:  return "NULL";
        case LogicalConstantKeyword::TRUE_KEYWORD:  return "TRUE";
        case LogicalConstantKeyword::FALSE_KEYWORD: return "FALSE";
        case LogicalConstantKeyword::UNKNOWN:       return "UNKNOWN";
        default:                                    return "UNKNOWN";
        }
    }

    static std::string TransactionKeywordTypeToString(TransactionKeyword kw) {
        switch (kw) {
        case TransactionKeyword::BEGIN:     return "BEGIN";
        case TransactionKeyword::COMMIT:    return "COMMIT";
        case TransactionKeyword::ROLLBACK:  return "ROLLBACK";
        case TransactionKeyword::SAVEPOINT: return "SAVEPOINT";
        case TransactionKeyword::RELEASE:   return "RELEASE";
        case TransactionKeyword::CHAIN:     return "CHAIN";
        case TransactionKeyword::UNKNOWN:   return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    static std::string SecurityKeywordTypeToString(SecurityKeyword kw) {
        switch (kw) {
        case SecurityKeyword::GRANT:   return "GRANT";
        case SecurityKeyword::REVOKE:  return "REVOKE";
        case SecurityKeyword::DENY:    return "DENY";
        case SecurityKeyword::ON:      return "ON";
        case SecurityKeyword::TO:      return "TO";
        case SecurityKeyword::UNKNOWN: return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    static std::string ProgStmtKeywordTypeToString(ProgStmtKeyword kw) {
        switch (kw) {
        case ProgStmtKeyword::DECLARE:  return "DECLARE";
        case ProgStmtKeyword::SET:      return "SET";
        case ProgStmtKeyword::PRINT:    return "PRINT";
        case ProgStmtKeyword::RETURN:   return "RETURN";
        case ProgStmtKeyword::THROW:    return "THROW";
        case ProgStmtKeyword::TRY:      return "TRY";
        case ProgStmtKeyword::CATCH:    return "CATCH";
        case ProgStmtKeyword::IF:       return "IF";
        case ProgStmtKeyword::ELSE:     return "ELSE";
        case ProgStmtKeyword::LOOP:     return "LOOP";
        case ProgStmtKeyword::WHILE:    return "WHILE";
        case ProgStmtKeyword::FOR:      return "FOR";
        case ProgStmtKeyword::BREAK:    return "BREAK";
        case ProgStmtKeyword::CONTINUE: return "CONTINUE";
        case ProgStmtKeyword::EXEC:     return "EXEC";
        case ProgStmtKeyword::GO:       return "GO";
        case ProgStmtKeyword::UNKNOWN:  return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    static std::string MiscKeywordTypeToString(MiscKeyword kw) {
        switch (kw) {
        case MiscKeyword::AS:           return "AS";
        case MiscKeyword::CASE:         return "CASE";
        case MiscKeyword::WHEN:         return "WHEN";
        case MiscKeyword::THEN:         return "THEN";
        case MiscKeyword::ELSE:         return "ELSE";
        case MiscKeyword::END:          return "END";
        case MiscKeyword::CAST:         return "CAST";
        case MiscKeyword::CONVERT:      return "CONVERT";
        case MiscKeyword::ASC:          return "ASC";
        case MiscKeyword::DESC:         return "DESC";
        case MiscKeyword::GENERATED:    return "GENERATED";
        case MiscKeyword::AUTOINCREMENT:return "AUTOINCREMENT";
        case MiscKeyword::CASCADE:      return "CASCADE";
        case MiscKeyword::RESTRICT:     return "RESTRICT";
        case MiscKeyword::DEFERRABLE:   return "DEFERRABLE";
        case MiscKeyword::EXPLAIN:      return "EXPLAIN";
        case MiscKeyword::ANALYZE:      return "ANALYZE";
        case MiscKeyword::VACUUM:       return "VACUUM";
        case MiscKeyword::UNKNOWN:      return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    static std::string FunctionCategoryTypeToString(FunctionCategory cat) {
        switch (cat) {
        case FunctionCategory::AGGREGATE:     return "AGGREGATE";
        case FunctionCategory::SCALAR:        return "SCALAR";
        case FunctionCategory::STRING:        return "STRING";
        case FunctionCategory::DATETIME:      return "DATETIME";
        case FunctionCategory::MATHEMATICAL:  return "MATHEMATICAL";
        case FunctionCategory::SYSTEM:        return "SYSTEM";
        case FunctionCategory::WINDOW:        return "WINDOW";
        case FunctionCategory::UNKNOWN:       return "UNKNOWN";
        default:                              return "UNKNOWN";
        }
    }

    static std::string AggregateFunctionTypeToString(AggregateFunction fn) {
        switch (fn) {
        case AggregateFunction::COUNT:       return "COUNT";
        case AggregateFunction::SUM:         return "SUM";
        case AggregateFunction::AVG:         return "AVG";
        case AggregateFunction::MIN:         return "MIN";
        case AggregateFunction::MAX:         return "MAX";
        case AggregateFunction::GROUP_CONCAT:return "GROUP_CONCAT";
        case AggregateFunction::ARRAY_AGG:   return "ARRAY_AGG";
        case AggregateFunction::LISTAGG:     return "LISTAGG";
        case AggregateFunction::STDDEV:      return "STDDEV";
        case AggregateFunction::VARIANCE:    return "VARIANCE";
        case AggregateFunction::UNKNOWN:     return "UNKNOWN";
        default:                             return "UNKNOWN";
        }
    }

    static std::string ScalarFunctionTypeToString(ScalarFunction fn) {
        switch (fn) {
        case ScalarFunction::CONVERT:   return "CONVERT";
        case ScalarFunction::CAST:      return "CAST";
        case ScalarFunction::COALESCE:  return "COALESCE";
        case ScalarFunction::NULLIF:    return "NULLIF";
        case ScalarFunction::IFNULL:    return "IFNULL";
        case ScalarFunction::LEAST:     return "LEAST";
        case ScalarFunction::GREATEST:  return "GREATEST";
        case ScalarFunction::FORMAT:    return "FORMAT";
        case ScalarFunction::LENGTH:    return "LENGTH";
        case ScalarFunction::POSITION:  return "POSITION";
        case ScalarFunction::ABS:       return "ABS";
        case ScalarFunction::ROUND:     return "ROUND";
        case ScalarFunction::FLOOR:     return "FLOOR";
        case ScalarFunction::CEILING:   return "CEILING";
        case ScalarFunction::DATE_TRUNC:return "DATE_TRUNC";
        case ScalarFunction::DATE_ADD:  return "DATE_ADD";
        case ScalarFunction::DATE_SUB:  return "DATE_SUB";
        case ScalarFunction::EXTRACT:   return "EXTRACT";
        case ScalarFunction::UNKNOWN:   return "UNKNOWN";
        default:                        return "UNKNOWN";
        }
    }

    static std::string StringFunctionTypeToString(StringFunction fn) {
        switch (fn) {
        case StringFunction::UPPER:            return "UPPER";
        case StringFunction::LOWER:            return "LOWER";
        case StringFunction::SUBSTRING:        return "SUBSTRING";
        case StringFunction::TRIM:             return "TRIM";
        case StringFunction::LTRIM:            return "LTRIM";
        case StringFunction::RTRIM:            return "RTRIM";
        case StringFunction::CONCAT:           return "CONCAT";
        case StringFunction::REPLACE:          return "REPLACE";
        case StringFunction::SPLIT_PART:       return "SPLIT_PART";
        case StringFunction::LEFT:             return "LEFT";
        case StringFunction::RIGHT:            return "RIGHT";
        case StringFunction::REPEAT:           return "REPEAT";
        case StringFunction::REVERSE:          return "REVERSE";
        case StringFunction::CHAR_LENGTH:      return "CHAR_LENGTH";
        case StringFunction::CHARACTER_LENGTH: return "CHARACTER_LENGTH";
        case StringFunction::POSITION:         return "POSITION";
        case StringFunction::UNKNOWN:          return "UNKNOWN";
        default:                               return "UNKNOWN";
        }
    }

    static std::string DateTimeFunctionTypeToString(DateTimeFunction fn) {
        switch (fn) {
        case DateTimeFunction::DATEPART:         return "DATEPART";
        case DateTimeFunction::GETDATE:          return "GETDATE";
        case DateTimeFunction::NOW:              return "NOW";
        case DateTimeFunction::CURRENT_DATE:     return "CURRENT_DATE";
        case DateTimeFunction::CURRENT_TIME:     return "CURRENT_TIME";
        case DateTimeFunction::CURRENT_TIMESTAMP:return "CURRENT_TIMESTAMP";
        case DateTimeFunction::LOCALTIME:        return "LOCALTIME";
        case DateTimeFunction::LOCALTIMESTAMP:   return "LOCALTIMESTAMP";
        case DateTimeFunction::AGE:              return "AGE";
        case DateTimeFunction::TO_DATE:          return "TO_DATE";
        case DateTimeFunction::TO_TIMESTAMP:     return "TO_TIMESTAMP";
        case DateTimeFunction::UNKNOWN:          return "UNKNOWN";
        default:                                 return "UNKNOWN";
        }
    }

    static std::string MathFunctionTypeToString(MathFunction fn) {
        switch (fn) {
        case MathFunction::ABS:      return "ABS";
        case MathFunction::CEILING:  return "CEILING";
        case MathFunction::FLOOR:    return "FLOOR";
        case MathFunction::ROUND:    return "ROUND";
        case MathFunction::POWER:    return "POWER";
        case MathFunction::SQRT:     return "SQRT";
        case MathFunction::EXP:      return "EXP";
        case MathFunction::LN:       return "LN";
        case MathFunction::LOG:      return "LOG";
        case MathFunction::MOD:      return "MOD";
        case MathFunction::RANDOM:   return "RANDOM";
        case MathFunction::SIGN:     return "SIGN";
        case MathFunction::TRUNC:    return "TRUNC";
        case MathFunction::PI:       return "PI";
        case MathFunction::SIN:      return "SIN";
        case MathFunction::COS:      return "COS";
        case MathFunction::TAN:      return "TAN";
        case MathFunction::ASIN:     return "ASIN";
        case MathFunction::ACOS:     return "ACOS";
        case MathFunction::ATAN:     return "ATAN";
        case MathFunction::DEGREES:  return "DEGREES";
        case MathFunction::RADIANS:  return "RADIANS";
        case MathFunction::UNKNOWN:  return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

    static std::string SystemFunctionTypeToString(SystemFunction fn) {
        switch (fn) {
        case SystemFunction::SUSER_SNAME:   return "SUSER_SNAME";
        case SystemFunction::CURRENT_USER:  return "CURRENT_USER";
        case SystemFunction::SESSION_USER:  return "SESSION_USER";
        case SystemFunction::USER:          return "USER";
        case SystemFunction::SYSTEM_USER:   return "SYSTEM_USER";
        case SystemFunction::TRANCOUNT:     return "TRANCOUNT";
        case SystemFunction::VERSION:       return "VERSION";
        case SystemFunction::DATABASE:      return "DATABASE";
        case SystemFunction::SCHEMA_NAME:   return "SCHEMA_NAME";
        case SystemFunction::UNKNOWN:       return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    static std::string WindowFunctionTypeToString(WindowFunction fn) {
        switch (fn) {
        case WindowFunction::ROW_NUMBER:    return "ROW_NUMBER";
        case WindowFunction::RANK:          return "RANK";
        case WindowFunction::DENSE_RANK:    return "DENSE_RANK";
        case WindowFunction::NTILE:         return "NTILE";
        case WindowFunction::LEAD:          return "LEAD";
        case WindowFunction::LAG:           return "LAG";
        case WindowFunction::FIRST_VALUE:   return "FIRST_VALUE";
        case WindowFunction::LAST_VALUE:    return "LAST_VALUE";
        case WindowFunction::NTH_VALUE:     return "NTH_VALUE";
        case WindowFunction::PERCENT_RANK:  return "PERCENT_RANK";
        case WindowFunction::CUME_DIST:     return "CUME_DIST";
        case WindowFunction::UNKNOWN:       return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

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

    static std::string AssignOpTypeToString(AssignOp op) {
        switch (op) {
        case AssignOp::ASSIGN:       return "ASSIGN";
        case AssignOp::COLON_ASSIGN: return "COLON_ASSIGN";
        case AssignOp::UNKNOWN:      return "UNKNOWN";
        default:                     return "UNKNOWN";
        }
    }

    static std::string ComparisonOpTypeToString(ComparisonOp op) {
        switch (op) {
        case ComparisonOp::LESS:                 return "LESS";
        case ComparisonOp::GREATER:              return "GREATER";
        case ComparisonOp::LESS_EQUAL:           return "LESS_EQUAL";
        case ComparisonOp::GREATER_EQUAL:        return "GREATER_EQUAL";
        case ComparisonOp::NOT_EQUAL:            return "NOT_EQUAL";
        case ComparisonOp::EQUAL:                return "EQUAL";
        case ComparisonOp::IS_DISTINCT_FROM:     return "IS_DISTINCT_FROM";
        case ComparisonOp::IS_NOT_DISTINCT_FROM: return "IS_NOT_DISTINCT_FROM";
        case ComparisonOp::LIKE:                 return "LIKE";
        case ComparisonOp::NOT_LIKE:             return "NOT_LIKE";
        case ComparisonOp::ILIKE:                return "ILIKE";
        case ComparisonOp::NOT_ILIKE:            return "NOT_ILIKE";
        case ComparisonOp::SIMILAR_TO:           return "SIMILAR_TO";
        case ComparisonOp::NOT_SIMILAR_TO:       return "NOT_SIMILAR_TO";
        case ComparisonOp::UNKNOWN:              return "UNKNOWN";
        default:                                 return "UNKNOWN";
        }
    }

    static std::string LogicalOpTypeToString(LogicalOp op) {
        switch (op) {
        case LogicalOp::AND:     return "AND";
        case LogicalOp::OR:      return "OR";
        case LogicalOp::NOT:     return "NOT";
        case LogicalOp::XOR:     return "XOR";
        case LogicalOp::IMPLIES: return "IMPLIES";
        case LogicalOp::UNKNOWN: return "UNKNOWN";
        default:                 return "UNKNOWN";
        }
    }

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

    static std::string ConcatOpTypeToString(ConcatOp op) {
        switch (op) {
        case ConcatOp::CONCAT:   return "CONCAT";
        case ConcatOp::UNKNOWN:  return "UNKNOWN";
        default:                 return "UNKNOWN";
        }
    }

    static std::string JsonOpTypeToString(JsonOp op) {
        switch (op) {
        case JsonOp::ARROW:        return "ARROW";
        case JsonOp::ARROW2:       return "ARROW2";
        case JsonOp::HASH_ARROW:   return "HASH_ARROW";
        case JsonOp::HASH_ARROW2:  return "HASH_ARROW2";
        case JsonOp::AT:           return "AT";
        case JsonOp::QUESTION:     return "QUESTION";
        case JsonOp::QUESTION_PIPE:return "QUESTION_PIPE";
        case JsonOp::QUESTION_AMP: return "QUESTION_AMP";
        case JsonOp::HASH_MINUS:   return "HASH_MINUS";
        case JsonOp::UNKNOWN:      return "UNKNOWN";
        default:                   return "UNKNOWN";
        }
    }

    static std::string RegexOpTypeToString(RegexOp op) {
        switch (op) {
        case RegexOp::TILDE:        return "TILDE";
        case RegexOp::NOT_TILDE:    return "NOT_TILDE";
        case RegexOp::TILDE_STAR:   return "TILDE_STAR";
        case RegexOp::NOT_TILDE_STAR:return "NOT_TILDE_STAR";
        case RegexOp::UNKNOWN:      return "UNKNOWN";
        default:                    return "UNKNOWN";
        }
    }

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
        case CommonSymbol::DOT:        return "DOT";
        case CommonSymbol::COLON:      return "COLON";
        case CommonSymbol::PARAM_MARKER:return "PARAM_MARKER";
        case CommonSymbol::UNKNOWN:    return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }

    static std::string TSQLSymbolTypeToString(TSQLSymbol sym) {
        switch (sym) {
        case TSQLSymbol::DOT:         return "DOT";
        case TSQLSymbol::COLON:       return "COLON";
        case TSQLSymbol::DOUBLE_COLON:return "DOUBLE_COLON";
        case TSQLSymbol::UNKNOWN:     return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    static std::string StringDelimiterTypeToString(StringDelimiter delim) {
        switch (delim) {
        case StringDelimiter::SINGLE_QUOTE: return "SINGLE_QUOTE";
        case StringDelimiter::DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        case StringDelimiter::BACKTICK:     return "BACKTICK";
        case StringDelimiter::DOLLAR_QUOTE: return "DOLLAR_QUOTE";
        case StringDelimiter::UNKNOWN:      return "UNKNOWN";
        default:                            return "UNKNOWN";
        }
    }

    static std::string commentTypeToString(CommentType ct) {
        switch (ct) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE:  return "MULTI_LINE";
        case CommentType::UNKNOWN:     return "UNKNOWN";
        default:                       return "UNKNOWN";
        }
    }
};