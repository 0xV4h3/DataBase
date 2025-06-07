#pragma once
#include <string>
#include <unordered_map>
#include "TokenEnums.hpp"

/**
 * @brief Utility class for lexical analysis operations.
 * @details
 * Provides utilities for:
 * - Token type conversions and validation
 * - Character classification
 * - Operator precedence management
 * - Symbol lookup and validation
 * - Error handling and reporting
 */
class LexerUtils {
public:
    // ====================== Type Conversion Methods ======================

    /**
     * @brief Convert token type enum to string representation.
     * @param type Token type to convert
     * @return String representation of the token type
     */
    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::KEYWORD:               return "KEYWORD";
        case TokenType::FUNCTION:              return "FUNCTION";
        case TokenType::IDENTIFIER:            return "IDENTIFIER";
        case TokenType::LITERAL:               return "LITERAL";
        case TokenType::LITERAL_CATEGORY:      return "LITERAL_CATEGORY";
        case TokenType::OPERATOR:              return "OPERATOR";
        case TokenType::PUNCTUATOR:            return "PUNCTUATOR";
        case TokenType::DATETIMEPART:          return "DATETIMEPART";
        case TokenType::COMMENT:               return "COMMENT";
        case TokenType::END_OF_FILE:           return "END_OF_FILE";
        case TokenType::UNKNOWN:               return "UNKNOWN";
        default:                               return "UNKNOWN";
        }
    }

    static std::string keywordCategoryToString(KeywordCategory cat) {
        switch (cat) {
        case KeywordCategory::DML:           return "DMLKeyword";
        case KeywordCategory::DDL:           return "DDLKeyword";
        case KeywordCategory::CLAUSE:        return "ClauseKeyword";
        case KeywordCategory::CTE:           return "CTEKeyword";
        case KeywordCategory::SETOP:         return "SetOpKeyword";
        case KeywordCategory::PREDICATE:     return "PredicateKeyword";  
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
        case OperatorCategory::ARITHMETIC: return "ArithmeticOp";
        case OperatorCategory::ASSIGN:     return "AssignOp";
        case OperatorCategory::COMPARISON: return "ComparisonOp";
        case OperatorCategory::LOGICAL:    return "LogicalOp";
        case OperatorCategory::BITWISE:    return "BitwiseOp";
        case OperatorCategory::CONCAT:     return "ConcatOp";
        case OperatorCategory::JSON:       return "JsonOp";
        case OperatorCategory::REGEX:      return "RegexOp";
        case OperatorCategory::ARRAY:      return "ArrayOp";
        case OperatorCategory::TYPECAST:   return "TypecastOp";
        case OperatorCategory::UNKNOWN:    return "UNKNOWN";
        default:                           return "UNKNOWN";
        }
    }

    static std::string literalCategoryToString(LiteralCategory type) {
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

    static std::string identifierCategoryToString(IdentifierCategory type) {
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
            // Core DML
        case DMLKeyword::SELECT:    return "SELECT";
        case DMLKeyword::INSERT:    return "INSERT";
        case DMLKeyword::UPDATE:    return "UPDATE";
        case DMLKeyword::DELETE:    return "DELETE";
            // Extended DML
        case DMLKeyword::MERGE:     return "MERGE";
        case DMLKeyword::EXECUTE:   return "EXECUTE";
        case DMLKeyword::VALUES:    return "VALUES";
            // Clause modifiers
        case DMLKeyword::OUTPUT:    return "OUTPUT";
        case DMLKeyword::DEFAULT:   return "DEFAULT";
        case DMLKeyword::INTO:      return "INTO";
        case DMLKeyword::RETURNING: return "RETURNING";
        case DMLKeyword::USING:     return "USING";
        case DMLKeyword::UNKNOWN:   return "UNKNOWN";
        default:                    return "UNKNOWN";
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
        case SetOpKeyword::UNION:     return "UNION";
        case SetOpKeyword::INTERSECT: return "INTERSECT";
        case SetOpKeyword::EXCEPT:    return "EXCEPT";
        case SetOpKeyword::UNKNOWN:   return "UNKNOWN";
        default:                      return "UNKNOWN";
        }
    }

    static std::string PredicateKeywordTypeToString(PredicateKeyword kw) {  
        switch (kw) {
        case PredicateKeyword::IN:      return "IN";
        case PredicateKeyword::IS:      return "IS";
        case PredicateKeyword::LIKE:    return "LIKE";
        case PredicateKeyword::BETWEEN: return "BETWEEN";
        case PredicateKeyword::EXISTS:  return "EXISTS";
        case PredicateKeyword::ALL:     return "ALL";
        case PredicateKeyword::ANY:     return "ANY";
        case PredicateKeyword::SOME:    return "SOME";
        case PredicateKeyword::NOT:     return "NOT";
        case PredicateKeyword::UNIQUE:  return "UNIQUE";
        case PredicateKeyword::UNKNOWN: return "UNKNOWN";
        default:                        return "UNKNOWN";
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
        case MiscKeyword::CASE:         return "CASE";
        case MiscKeyword::WHEN:         return "WHEN";
        case MiscKeyword::THEN:         return "THEN";
        case MiscKeyword::ELSE:         return "ELSE";
        case MiscKeyword::END:          return "END";
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
        case FunctionCategory::AGGREGATE:    return "AGGREGATE";
        case FunctionCategory::SCALAR:       return "SCALAR";
        case FunctionCategory::STRING:       return "STRING";
        case FunctionCategory::DATETIME:     return "DATETIME";
        case FunctionCategory::MATHEMATICAL: return "MATHEMATICAL";
        case FunctionCategory::SYSTEM:       return "SYSTEM";
        case FunctionCategory::WINDOW:       return "WINDOW";
        case FunctionCategory::UNKNOWN:      return "UNKNOWN";
        default:                             return "UNKNOWN";
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
            // Basic comparisons
        case ComparisonOp::LESS:                 return "LESS";
        case ComparisonOp::GREATER:              return "GREATER";
        case ComparisonOp::LESS_EQUAL:           return "LESS_EQUAL";
        case ComparisonOp::GREATER_EQUAL:        return "GREATER_EQUAL";
        case ComparisonOp::NOT_EQUAL:            return "NOT_EQUAL";
        case ComparisonOp::EQUAL:                return "EQUAL";
            // Null-safe equality
        case ComparisonOp::IS_DISTINCT_FROM:     return "IS_DISTINCT_FROM";
        case ComparisonOp::IS_NOT_DISTINCT_FROM: return "IS_NOT_DISTINCT_FROM";
            // Pattern matching
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

    static std::string TypecastOpTypeToString(TypecastOp op) {
        switch (op) {
        case TypecastOp::TYPECAST:  return "TYPECAST";
        case TypecastOp::UNKNOWN:   return "UNKNOWN";
        default:                    return "UNKNOWN";
        }
    }

    static std::string DateTimePartTypeToString(DateTimePart part) {
        switch (part) {
            // Date parts
        case DateTimePart::YEAR:            return "YEAR";
        case DateTimePart::QUARTER:         return "QUARTER";
        case DateTimePart::MONTH:           return "MONTH";
        case DateTimePart::DAY_OF_YEAR:     return "DAY_OF_YEAR";
        case DateTimePart::DAY:             return "DAY";
        case DateTimePart::WEEK:            return "WEEK";
        case DateTimePart::ISO_WEEK:        return "ISO_WEEK";
        case DateTimePart::WEEKDAY:         return "WEEKDAY";
            // Time parts
        case DateTimePart::HOUR:            return "HOUR";
        case DateTimePart::MINUTE:          return "MINUTE";
        case DateTimePart::SECOND:          return "SECOND";
        case DateTimePart::MILLISECOND:     return "MILLISECOND";
        case DateTimePart::MICROSECOND:     return "MICROSECOND";
        case DateTimePart::NANOSECOND:      return "NANOSECOND";
            // Zone
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

    // ====================== Character Classification ======================

    /**
     * @brief Check if character is valid in SQL identifiers.
     * @param c Character to check
     * @return true if character can be used in identifiers
     */
    static bool isIdentifierChar(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    /**
     * @brief Check if character can start an SQL identifier.
     * @param c Character to check
     * @return true if character can start an identifier
     */
    static bool isIdentifierStart(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '@';
    }

    /**
     * @brief Check if character is a valid operator symbol.
     * @param c Character to check
     * @return true if character is an operator symbol
     */
    static bool isOperatorChar(char c) {
        return std::string("=!<>+-*/%^&|~:").find(c) != std::string::npos;
    }

    /**
     * @brief Check if character is whitespace (including SQL-specific).
     * @param c Character to check
     * @return true if character is whitespace
     */
    static bool isWhitespace(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }

    // ====================== Operator Precedence ======================

    /**
     * @brief Get operator precedence for a given operator type.
     * @param op Operator type
     * @return Precedence value (higher = binds tighter)
     */
    static int getOperatorPrecedence(OperatorCategory op) {
        switch (op) {
        case OperatorCategory::ARITHMETIC:  return SQLOperatorPrecedence::MULTIPLICATIVE;
        case OperatorCategory::BITWISE:     return SQLOperatorPrecedence::BITWISE_AND;
        case OperatorCategory::COMPARISON:  return SQLOperatorPrecedence::COMPARISON;
        case OperatorCategory::LOGICAL:     return SQLOperatorPrecedence::AND;
        case OperatorCategory::ASSIGN:      return SQLOperatorPrecedence::ASSIGNMENT;
        case OperatorCategory::CONCAT:      return SQLOperatorPrecedence::ADDITIVE;
        case OperatorCategory::TYPECAST:    return SQLOperatorPrecedence::TYPECAST;
        default:                            return SQLOperatorPrecedence::LOWEST;
        }
    }

    /**
     * @brief Check if operator is left associative.
     * @param op Operator type
     * @return true if operator is left associative
     */
    static bool isLeftAssociative(OperatorCategory op) {
        switch (op) {
        case OperatorCategory::ARITHMETIC:  return true;
        case OperatorCategory::BITWISE:     return true;
        case OperatorCategory::COMPARISON:  return true;
        case OperatorCategory::LOGICAL:     return true;
        case OperatorCategory::CONCAT:      return true;
        case OperatorCategory::ASSIGN:      return false;
        case OperatorCategory::TYPECAST:    return true; 
        default:                            return true;
        }
    }

    // ====================== Operator Methods ======================

    /**
   * @brief Get the string representation of operator symbols for a category.
   * @param op Operator category to get symbols for
   * @return String containing all symbols for the category
   */
    static std::string getOperatorSymbol(OperatorCategory op) {
        switch (op) {
        case OperatorCategory::ARITHMETIC:  return "+-*/%";
        case OperatorCategory::BITWISE:     return "&|^~";
        case OperatorCategory::COMPARISON:  return "<>=!";
        case OperatorCategory::LOGICAL:     return "AND OR NOT";
        case OperatorCategory::CONCAT:      return "||";
        case OperatorCategory::TYPECAST:    return "::";
        default:                            return "";
        }
    }

    /**
     * @brief Check if operator category represents unary operators.
     * @param op Operator category to check
     * @return true if category contains unary operators
     */
    static bool isUnaryOperator(OperatorCategory op) {
        return op == OperatorCategory::ARITHMETIC || op == OperatorCategory::LOGICAL;
    }

    /**
     * @brief Check if operator category represents binary operators.
     * @param op Operator category to check
     * @return true if category contains binary operators
     */
    static bool isBinaryOperator(OperatorCategory op) {
        return op != OperatorCategory::UNKNOWN &&
            (op == OperatorCategory::ARITHMETIC ||
                op == OperatorCategory::BITWISE ||
                op == OperatorCategory::COMPARISON ||
                op == OperatorCategory::LOGICAL ||
                op == OperatorCategory::ASSIGN ||
                op == OperatorCategory::CONCAT ||
                op == OperatorCategory::JSON ||
                op == OperatorCategory::REGEX ||
                op == OperatorCategory::ARRAY ||
                op == OperatorCategory::TYPECAST);
    }

    /**
     * @brief Get the string representation of JSON operator symbols.
     * @param op JSON operator to get symbol for
     * @return String containing the operator symbol
     */
    static std::string getJsonOperatorSymbol(JsonOp op) {
        switch (op) {
        case JsonOp::ARROW:         return "->";     // Path navigation
        case JsonOp::ARROW2:        return "->>";    // Text extraction
        case JsonOp::HASH_ARROW:    return "#>";     // Path navigation (array)
        case JsonOp::HASH_ARROW2:   return "#>>";    // Text extraction (array)
        case JsonOp::AT:            return "@";      // Array index
        case JsonOp::QUESTION:      return "?";      // Key existence
        case JsonOp::QUESTION_PIPE: return "?|";     // Any key exists
        case JsonOp::QUESTION_AMP:  return "?&";     // All keys exist
        case JsonOp::HASH_MINUS:    return "#-";     // Delete key
        default:                    return "";
        }
    }

    // ====================== Symbol Lookup ======================

    /**
     * @brief Get the string representation of a symbol.
     * @param sym Symbol type
     * @return Symbol string
     */
    static std::string getSymbolString(CommonSymbol sym) {
        switch (sym) {
        case CommonSymbol::COMMA:       return ",";
        case CommonSymbol::SEMICOLON:   return ";";
        case CommonSymbol::LPAREN:      return "(";
        case CommonSymbol::RPAREN:      return ")";
        case CommonSymbol::LBRACE:      return "{";
        case CommonSymbol::RBRACE:      return "}";
        case CommonSymbol::LBRACKET:    return "[";
        case CommonSymbol::RBRACKET:    return "]";
        case CommonSymbol::DOT:         return ".";
        case CommonSymbol::COLON:       return ":";
        case CommonSymbol::PARAM_MARKER: return "?";
        default:                        return "";
        }
    }

    // ====================== Validation Methods ======================

    /**
     * @brief Check if token type is valid.
     * @param type Token type to validate
     * @return true if token type is valid
     */
    static bool isValidTokenType(TokenType type) {
        return type != TokenType::UNKNOWN;
    }

    /**
     * @brief Check if keyword category is valid.
     * @param cat Keyword category to validate
     * @return true if category is valid
     */
    static bool isValidKeywordCategory(KeywordCategory cat) {
        return cat != KeywordCategory::UNKNOWN;
    }

    /**
     * @brief Check if operator category is valid.
     * @param cat Operator category to validate
     * @return true if category is valid
     */
    static bool isValidOperatorCategory(OperatorCategory cat) {
        return cat != OperatorCategory::UNKNOWN;
    }

    // ====================== Error Reporting ======================

    /**
     * @brief Get error message for invalid token.
     * @param type Invalid token type
     * @param pos Position in input
     * @return Error message string
     */
    static std::string getInvalidTokenMessage(TokenType type, size_t pos) {
        return "Invalid token of type " + tokenTypeToString(type) + " at position " + std::to_string(pos);
    }

    /**
     * @brief Get error message for unexpected character.
     * @param c Unexpected character
     * @param pos Position in input
     * @return Error message string
     */
    static std::string getUnexpectedCharMessage(char c, size_t pos) {
        return "Unexpected character '" + std::string(1, c) + "' at position " + std::to_string(pos);
    }
};