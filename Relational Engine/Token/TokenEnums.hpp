#pragma once

/**
 * @file TokenEnums.hpp
 * @brief Enumerations for all token types, keyword/function/operator categories, and related SQL parsing constants.
 * @details
 * This file contains enums for token types, SQL keyword and function categories,
 * identifier types, literal types, operators, date/time parts, punctuators, and comments.
 * Used throughout the lexer, parser, and semantic analysis modules.
 */

 // ---------------- Token types ----------------

 /**
  * @enum TokenType
  * @brief Types of tokens produced by the lexer.
  */
enum class TokenType {
    UNKNOWN = 0,
    KEYWORD,
    FUNCTION,
    IDENTIFIER,
    LITERAL,
    OPERATOR,
    PUNCTUATOR,
    DATETIMEPART,
    COMMENT,
    END_OF_FILE
};

// ---------------- Keyword categories and their enums ----------------

/**
 * @enum DMLKeyword
 * @brief Data Manipulation Language keywords.
 */
enum class DMLKeyword {
    UNKNOWN = 0,
    SELECT, INSERT, UPDATE, DELETE, MERGE, EXECUTE, VALUES, OUTPUT, DEFAULT, INTO, RETURNING, USING
};

/**
 * @enum DDLKeyword
 * @brief Data Definition Language keywords.
 */
enum class DDLKeyword {
    UNKNOWN = 0,
    CREATE, ALTER, DROP, TRUNCATE, TABLE, VIEW, INDEX, SEQUENCE, CONSTRAINT, TRIGGER, PRIMARY, FOREIGN, REFERENCES,
    UNIQUE, CHECK, PARTITION, COLUMN, DATABASE, SCHEMA, TYPE
};

/**
 * @enum ClauseKeyword
 * @brief SQL clause keywords.
 */
enum class ClauseKeyword {
    UNKNOWN = 0,
    FROM, WHERE, JOIN, ON, GROUP, BY, HAVING, ORDER, UNION, DISTINCT, TOP, LIMIT, OFFSET, LEFT, RIGHT, FULL,
    OUTER, INNER, CROSS, APPLY, WINDOW, PARTITION, OVER, AS, USING, DO, END
};

/**
 * @enum CTEKeyword
 * @brief Common Table Expression keywords.
 */
enum class CTEKeyword {
    UNKNOWN = 0,
    WITH, RECURSIVE
};

/**
 * @enum SetOpKeyword
 * @brief Set operation keywords (UNION, INTERSECT, etc).
 */
enum class SetOpKeyword {
    UNKNOWN = 0,
    EXCEPT, INTERSECT, UNION
};

/**
 * @enum WordOperatorKeyword
 * @brief Word-like operator keywords.
 */
enum class WordOperatorKeyword {
    UNKNOWN = 0,
    IN, IS, NOT, LIKE, BETWEEN, EXISTS, ALL, ANY, SOME, UNIQUE
};

/**
 * @enum LogicalConstantKeyword
 * @brief Logical constant keywords (NULL, TRUE, FALSE).
 */
enum class LogicalConstantKeyword {
    UNKNOWN = 0,
    NULL_KEYWORD, TRUE_KEYWORD, FALSE_KEYWORD
};

/**
 * @enum TransactionKeyword
 * @brief Transaction control keywords.
 */
enum class TransactionKeyword {
    UNKNOWN = 0,
    BEGIN, COMMIT, ROLLBACK, SAVEPOINT, RELEASE, CHAIN
};

/**
 * @enum SecurityKeyword
 * @brief Security-related keywords.
 */
enum class SecurityKeyword {
    UNKNOWN = 0,
    GRANT, REVOKE, DENY, ON, TO
};

/**
 * @enum ProgStmtKeyword
 * @brief Programming statement keywords (procedural, control flow, etc).
 */
enum class ProgStmtKeyword {
    UNKNOWN = 0,
    DECLARE, SET, PRINT, RETURN, THROW, TRY, CATCH, IF, ELSE, LOOP, WHILE, FOR, BREAK, CONTINUE, EXEC, GO
};

/**
 * @enum MiscKeyword
 * @brief Miscellaneous SQL keywords.
 */
enum class MiscKeyword {
    UNKNOWN = 0,
    AS, CASE, WHEN, THEN, ELSE, END, CAST, CONVERT, ASC, DESC, GENERATED, AUTOINCREMENT, CASCADE, RESTRICT, DEFERRABLE,
    EXPLAIN, ANALYZE, VACUUM
};

// ---------------- Function categories and their enums ----------------

/**
 * @enum AggregateFunction
 * @brief Aggregate SQL functions.
 */
enum class AggregateFunction {
    UNKNOWN = 0,
    COUNT, SUM, AVG, MIN, MAX, GROUP_CONCAT, ARRAY_AGG, LISTAGG, STDDEV, VARIANCE
};

/**
 * @enum ScalarFunction
 * @brief Scalar SQL functions.
 */
enum class ScalarFunction {
    UNKNOWN = 0,
    CONVERT, CAST, COALESCE, NULLIF, IFNULL, LEAST, GREATEST, FORMAT, LENGTH, POSITION, ABS, ROUND, FLOOR, CEILING,
    DATE_TRUNC, DATE_ADD, DATE_SUB, EXTRACT
};

/**
 * @enum StringFunction
 * @brief String manipulation SQL functions.
 */
enum class StringFunction {
    UNKNOWN = 0,
    UPPER, LOWER, SUBSTRING, TRIM, LTRIM, RTRIM, CONCAT, REPLACE, SPLIT_PART, LEFT, RIGHT, REPEAT, REVERSE,
    CHAR_LENGTH, CHARACTER_LENGTH, POSITION
};

/**
 * @enum DateTimeFunction
 * @brief Date/time SQL functions.
 */
enum class DateTimeFunction {
    UNKNOWN = 0,
    DATEPART, GETDATE, NOW, CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP, LOCALTIME, LOCALTIMESTAMP, AGE,
    TO_DATE, TO_TIMESTAMP
};

/**
 * @enum MathFunction
 * @brief Mathematical SQL functions.
 */
enum class MathFunction {
    UNKNOWN = 0,
    ABS, CEILING, FLOOR, ROUND, POWER, SQRT, EXP, LN, LOG, MOD, RANDOM, SIGN, TRUNC, PI, SIN, COS, TAN, ASIN, ACOS, ATAN,
    DEGREES, RADIANS
};

/**
 * @enum SystemFunction
 * @brief System SQL functions.
 */
enum class SystemFunction {
    UNKNOWN = 0,
    SUSER_SNAME, CURRENT_USER, SESSION_USER, USER, SYSTEM_USER, TRANCOUNT, VERSION, DATABASE, SCHEMA_NAME
};

/**
 * @enum WindowFunction
 * @brief SQL window functions.
 */
enum class WindowFunction {
    UNKNOWN = 0,
    ROW_NUMBER, RANK, DENSE_RANK, NTILE, LEAD, LAG, FIRST_VALUE, LAST_VALUE, NTH_VALUE, PERCENT_RANK, CUME_DIST
};

// ---------------- Category enums for token types ----------------

/**
 * @enum KeywordCategory
 * @brief Keyword category for tokens.
 */
enum class KeywordCategory {
    UNKNOWN = 0,
    DML, DDL, CLAUSE, CTE, SETOP, WORD_OP, LOGICAL_CONST,
    TRANSACTION, SECURITY, PROG_STMT, MISC
};

/**
 * @enum FunctionCategory
 * @brief Function category for tokens.
 */
enum class FunctionCategory {
    UNKNOWN = 0,
    AGGREGATE, SCALAR, STRING, DATETIME, MATHEMATICAL, SYSTEM, WINDOW
};

/**
 * @enum OperatorCategory
 * @brief Operator category for tokens.
 */
enum class OperatorCategory {
    UNKNOWN = 0,
    ARITHMETIC, ASSIGN, COMPARISON, LOGICAL, BITWISE, CONCAT, JSON, REGEX, ARRAY, TYPECAST
};

/**
 * @enum PunctuatorCategory
 * @brief Punctuator/delimiter category.
 */
enum class PunctuatorCategory {
    UNKNOWN = 0,
    COMMON, TSQL, STRING_DELIM, DOLLAR_QUOTE, PARAM_MARKER
};

// ---------------- Identifier category enums ----------------

/**
 * @enum IdentifierCategory
 * @brief Identifier category for tokens.
 */
enum class IdentifierCategory {
    UNKNOWN = 0,
    TABLE, VIEW, PROCEDURE, FUNCTION, TRIGGER, INDEX, CONSTRAINT,
    SCHEMA, DATABASE, SEQUENCE, USER_DEFINED_TYPE, ROLE, USER, EXTERNAL_TABLE,
    USER_VARIABLE, SYSTEM_VARIABLE, TEMP_TABLE, GLOBAL_TEMP_TABLE,
    COLUMN, PARAMETER, LABEL
};

// ---------------- Literal category enums ----------------

/**
 * @enum LiteralCategory
 * @brief Literal value category for tokens.
 */
enum class LiteralCategory {
    UNKNOWN = 0,
    STRING, ESCAPE_STRING, CHAR, INTEGER, FLOAT, BINARY, HEX,
    DATE, TIME, DATETIME, INTERVAL, UUID, ARRAY, JSON, XML, BOOLEAN, NULL_VALUE
};

// ---------------- Operator enums ----------------

/**
 * @enum ArithmeticOp
 * @brief Arithmetic operator.
 */
enum class ArithmeticOp { UNKNOWN = 0, PLUS, MINUS, MULTIPLY, DIVIDE, MOD };

/**
 * @enum AssignOp
 * @brief Assignment operator.
 */
enum class AssignOp { UNKNOWN = 0, ASSIGN, COLON_ASSIGN };

/**
 * @enum ComparisonOp
 * @brief Comparison operator.
 */
enum class ComparisonOp {
    UNKNOWN = 0,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, NOT_EQUAL, EQUAL, IS_DISTINCT_FROM, IS_NOT_DISTINCT_FROM,
    LIKE, NOT_LIKE, ILIKE, NOT_ILIKE, SIMILAR_TO, NOT_SIMILAR_TO
};

/**
 * @enum LogicalOp
 * @brief Logical operator.
 */
enum class LogicalOp { UNKNOWN = 0, AND, OR, NOT, XOR, IMPLIES };

/**
 * @enum BitwiseOp
 * @brief Bitwise operator.
 */
enum class BitwiseOp {
    UNKNOWN = 0,
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LEFT_SHIFT, RIGHT_SHIFT
};

/**
 * @enum ConcatOp
 * @brief Concatenation operator.
 */
enum class ConcatOp { UNKNOWN = 0, CONCAT };

/**
 * @enum JsonOp
 * @brief JSON operator.
 */
enum class JsonOp { UNKNOWN = 0, ARROW, ARROW2, HASH_ARROW, HASH_ARROW2, AT, QUESTION, QUESTION_PIPE, QUESTION_AMP, HASH_MINUS };

/**
 * @enum RegexOp
 * @brief Regular expression operator.
 */
enum class RegexOp { UNKNOWN = 0, TILDE, NOT_TILDE, TILDE_STAR, NOT_TILDE_STAR };

// ---------------- Date/Time part identifiers ----------------

/**
 * @enum DateTimePart
 * @brief Identifiers for SQL date/time parts.
 */
enum class DateTimePart {
    UNKNOWN = 0,
    YEAR, QUARTER, MONTH, DAY_OF_YEAR, DAY, WEEK, ISO_WEEK, WEEKDAY,
    HOUR, MINUTE, SECOND, MILLISECOND, MICROSECOND, NANOSECOND, TIMEZONE_OFFSET
};

// ---------------- Punctuator and delimiter enums ----------------

/**
 * @enum CommonSymbol
 * @brief Common SQL punctuation symbols.
 */
enum class CommonSymbol {
    UNKNOWN = 0,
    COMMA, SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, DOT, COLON, PARAM_MARKER
};

/**
 * @enum TSQLSymbol
 * @brief TSQL-specific symbols.
 */
enum class TSQLSymbol { UNKNOWN = 0, DOT, COLON, DOUBLE_COLON };

/**
 * @enum StringDelimiter
 * @brief String delimiter types.
 */
enum class StringDelimiter { UNKNOWN = 0, SINGLE_QUOTE, DOUBLE_QUOTE, BACKTICK, DOLLAR_QUOTE };

// ---------------- Comment category enum ----------------

/**
 * @enum CommentType
 * @brief SQL comment type.
 */
enum class CommentType {
    UNKNOWN = 0,
    SINGLE_LINE,
    MULTI_LINE
};