#pragma once

/**
 * @file TokenEnums.hpp
 * @brief Enumerations for all token types, keyword/function/operator categories, and related SQL parsing constants.
 * @details
 * This file contains enums for token types, SQL keyword and function categories,
 * identifier types, literal types, operators, date/time parts, punctuators, and comments.
 * Used throughout the lexer, parser, and semantic analysis modules.
 */

 // ---------------- Operator Precedence Constants ----------------

namespace SQLOperatorPrecedence {
    constexpr int HIGHEST = 100;       // Parentheses, brackets
    constexpr int MEMBER = 90;         // . and -> operators
    constexpr int TYPECAST = 85;       // ::
    constexpr int UNARY = 80;          // NOT, !, -, +, ~
    constexpr int MULTIPLICATIVE = 70; // *, /, %
    constexpr int ADDITIVE = 60;       // +, -
    constexpr int SHIFT = 55;          // <<, >>
    constexpr int BITWISE_AND = 50;    // &
    constexpr int BITWISE_XOR = 45;    // ^
    constexpr int BITWISE_OR = 40;     // |
    constexpr int COMPARISON = 35;     // =, <>, <, >, <=, >=
    constexpr int NULL_TEST = 30;      // IS NULL, IS NOT NULL
    constexpr int BETWEEN = 25;        // BETWEEN, NOT BETWEEN
    constexpr int IN = 20;             // IN, NOT IN
    constexpr int PATTERN = 15;        // LIKE, SIMILAR TO, REGEX
    constexpr int AND = 10;            // AND
    constexpr int OR = 5;              // OR
    constexpr int ASSIGNMENT = 1;      // =, :=
    constexpr int LOWEST = 0;          // Comma operator
}

// ---------------- Core Token Types ----------------

/**
 * @enum TokenType
 * @brief Fundamental token types produced by lexical analysis.
 */
enum class TokenType {
    UNKNOWN = 0,
    KEYWORD,         // SQL keywords (SELECT, INSERT, etc)
    FUNCTION,        // Built-in functions (COUNT, MAX, etc)
    IDENTIFIER,      // Names (tables, columns, aliases)
    LITERAL,         // Constants (numbers, strings, etc)
    LITERAL_CATEGORY,//  Literal category (INTEGER, JSON, XML…)
    OPERATOR,        // Operations (+, -, *, /, etc)
    PUNCTUATOR,      // Syntax elements (,;(){}[], etc)
    DATETIMEPART,    // Date/time components (YEAR, MONTH, etc)
    COMMENT,         // SQL comments
    END_OF_FILE      // End of input marker
};

// ---------------- SQL Statement Keywords ----------------

/**
 * @enum DMLKeyword
 * @brief Data Manipulation Language keywords.
 */
enum class DMLKeyword {
    UNKNOWN = 0,
    // Core DML
    SELECT, INSERT, UPDATE, DELETE,
    // Extended DML
    MERGE, EXECUTE, VALUES,
    // Clause modifiers
    OUTPUT, DEFAULT, INTO, RETURNING, USING
};

/**
 * @enum DDLKeyword
 * @brief Data Definition Language keywords.
 */
enum class DDLKeyword {
    UNKNOWN = 0,
    // Core DDL actions
    CREATE, ALTER, DROP, TRUNCATE,
    // Object types
    TABLE, VIEW, INDEX, SEQUENCE, CONSTRAINT, TRIGGER,
    // Constraint types
    PRIMARY, FOREIGN, REFERENCES, UNIQUE, CHECK,
    // Object attributes
    PARTITION, COLUMN, DATABASE, SCHEMA, TYPE
};

/**
 * @enum ClauseKeyword
 * @brief SQL clause keywords.
 */
enum class ClauseKeyword {
    UNKNOWN = 0,
    // Core clauses
    FROM, WHERE, GROUP, HAVING, ORDER,
    // Join types
    JOIN, INNER, LEFT, RIGHT, FULL, CROSS,
    // Join modifiers
    OUTER, ON, USING,
    // Result modifiers
    DISTINCT, TOP, LIMIT, OFFSET,
    // Window clauses
    WINDOW, PARTITION, OVER,
    // Other modifiers
    AS, BY, DO, END,
    // Special
    UNION, APPLY
};

/**
 * @enum CTEKeyword
 * @brief Common Table Expression keywords.
 */
enum class CTEKeyword {
    UNKNOWN = 0,
    WITH,
    RECURSIVE
};

/**
 * @enum SetOpKeyword
 * @brief Set operation keywords.
 */
enum class SetOpKeyword {
    UNKNOWN = 0,
    UNION,      // Combines and removes duplicates
    INTERSECT,  // Common rows only
    EXCEPT      // First set minus second set
};

/**
 * @enum PredicateKeyword
 * @brief Predicate and condition keywords.
 * @details Replaces WordOperatorKeyword for clarity.
 */
enum class PredicateKeyword {
    UNKNOWN = 0,
    // Comparison predicates
    IN, IS, LIKE, BETWEEN,
    // Quantifiers
    ALL, ANY, SOME,
    // Existence test
    EXISTS,
    // Modifiers
    NOT,
    // Constraints
    UNIQUE
};

/**
 * @enum LogicalConstantKeyword
 * @brief Logical and special constant keywords.
 */
enum class LogicalConstantKeyword {
    UNKNOWN = 0,
    NULL_KEYWORD,   // NULL value
    TRUE_KEYWORD,   // Boolean true
    FALSE_KEYWORD   // Boolean false
};

/**
 * @enum TransactionKeyword
 * @brief Transaction control keywords.
 */
enum class TransactionKeyword {
    UNKNOWN = 0,
    // Core transaction
    BEGIN, COMMIT, ROLLBACK,
    // Savepoint control
    SAVEPOINT, RELEASE,
    // Transaction options
    CHAIN
};

/**
 * @enum SecurityKeyword
 * @brief Security and permission keywords.
 */
enum class SecurityKeyword {
    UNKNOWN = 0,
    GRANT,    // Give permissions
    REVOKE,   // Remove permissions
    DENY,     // Explicitly deny
    ON,       // Target specifier
    TO        // Recipient specifier
};

/**
 * @enum ProgStmtKeyword
 * @brief Programming and control flow keywords.
 */
enum class ProgStmtKeyword {
    UNKNOWN = 0,
    // Variable handling
    DECLARE, SET,
    // Output
    PRINT, RETURN,
    // Error handling
    THROW, TRY, CATCH,
    // Control flow
    IF, ELSE, LOOP, WHILE, FOR,
    // Flow control
    BREAK, CONTINUE,
    // Execution control
    EXEC, GO
};

/**
 * @enum MiscKeyword
 * @brief Miscellaneous SQL keywords.
 */
enum class MiscKeyword {
    UNKNOWN = 0,
    // Flow control
    CASE, WHEN, THEN, ELSE, END,
    // Sort direction
    ASC, DESC,
    // Column attributes
    GENERATED, AUTOINCREMENT,
    // Referential actions
    CASCADE, RESTRICT,
    // Constraint timing
    DEFERRABLE,
    // Query analysis
    EXPLAIN, ANALYZE,
    // Maintenance
    VACUUM
};

// ---------------- Function Categories ----------------

/**
 * @enum FunctionCategory
 * @brief High-level categorization of SQL functions.
 */
enum class FunctionCategory {
    UNKNOWN = 0,
    AGGREGATE,    // Group operations
    SCALAR,       // Single-row functions
    STRING,       // Text manipulation
    DATETIME,     // Date/time handling
    MATHEMATICAL, // Numeric operations
    SYSTEM,       // Database/system info
    WINDOW        // Window/analytic functions
};

/**
 * @enum AggregateFunction
 * @brief Aggregate SQL functions.
 */
enum class AggregateFunction {
    UNKNOWN = 0,
    // Basic aggregates
    COUNT, SUM, AVG, MIN, MAX,
    // String aggregates
    GROUP_CONCAT, LISTAGG,
    // Array operations
    ARRAY_AGG,
    // Statistical
    STDDEV, VARIANCE
};

/**
 * @enum ScalarFunction
 * @brief Scalar SQL functions.
 */
enum class ScalarFunction {
    UNKNOWN = 0,
    // Type conversion
    CONVERT, CAST,
    // Null handling
    COALESCE, NULLIF, IFNULL,
    // Comparison
    LEAST, GREATEST,
    // Formatting
    FORMAT,
    // String operations
    LENGTH, POSITION,
    // Numeric
    ABS, ROUND, FLOOR, CEILING,
    // Date manipulation
    DATE_TRUNC, DATE_ADD, DATE_SUB, EXTRACT
};

/**
 * @enum StringFunction
 * @brief String manipulation functions.
 */
enum class StringFunction {
    UNKNOWN = 0,
    // Case conversion
    UPPER, LOWER,
    // Extraction
    SUBSTRING,
    // Trimming
    TRIM, LTRIM, RTRIM,
    // Combination
    CONCAT, REPLACE,
    // Parts
    SPLIT_PART,
    // Position
    LEFT, RIGHT,
    // Modification
    REPEAT, REVERSE,
    // Length
    CHAR_LENGTH, CHARACTER_LENGTH,
    // Search
    POSITION
};

/**
 * @enum DateTimeFunction
 * @brief Date/time functions.
 */
enum class DateTimeFunction {
    UNKNOWN = 0,
    // Parts
    DATEPART,
    // Current timestamp
    GETDATE, NOW,
    // Current components
    CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP,
    // Local time
    LOCALTIME, LOCALTIMESTAMP,
    // Calculations
    AGE,
    // Conversions
    TO_DATE, TO_TIMESTAMP
};

/**
 * @enum MathFunction
 * @brief Mathematical functions.
 */
enum class MathFunction {
    UNKNOWN = 0,
    // Basic
    ABS, CEILING, FLOOR, ROUND,
    // Advanced
    POWER, SQRT, EXP, LN, LOG,
    // Integer
    MOD, SIGN, TRUNC,
    // Random
    RANDOM,
    // Constants
    PI,
    // Trigonometry
    SIN, COS, TAN, ASIN, ACOS, ATAN,
    // Conversion
    DEGREES, RADIANS
};

/**
 * @enum SystemFunction
 * @brief System and metadata functions.
 */
enum class SystemFunction {
    UNKNOWN = 0,
    // User info
    SUSER_SNAME, CURRENT_USER, SESSION_USER,
    USER, SYSTEM_USER,
    // Transaction
    TRANCOUNT,
    // System info
    VERSION, DATABASE, SCHEMA_NAME
};

/**
 * @enum WindowFunction
 * @brief Window/analytic functions.
 */
enum class WindowFunction {
    UNKNOWN = 0,
    // Ranking
    ROW_NUMBER, RANK, DENSE_RANK,
    // Distribution
    NTILE, PERCENT_RANK, CUME_DIST,
    // Navigation
    LEAD, LAG,
    // Value access
    FIRST_VALUE, LAST_VALUE, NTH_VALUE
};

// ---------------- Categories and Types ----------------

/**
 * @enum KeywordCategory
 * @brief High-level keyword classification.
 */
enum class KeywordCategory {
    UNKNOWN = 0,
    DML,           // Data manipulation
    DDL,           // Data definition
    CLAUSE,        // Query clauses
    CTE,           // Common table expressions
    SETOP,         // Set operations
    PREDICATE,     // Conditions and tests
    LOGICAL_CONST, // TRUE, FALSE, NULL
    TRANSACTION,   // Transaction control
    SECURITY,      // Permissions
    PROG_STMT,     // Programming
    MISC          // Other keywords
};

/**
 * @enum OperatorCategory
 * @brief Operator classification.
 */
enum class OperatorCategory {
    UNKNOWN = 0,
    ARITHMETIC,  // Math operations
    ASSIGN,      // Assignment
    COMPARISON,  // Comparisons
    LOGICAL,     // Boolean operations
    BITWISE,     // Bit manipulation
    CONCAT,      // String concatenation
    JSON,        // JSON operations
    REGEX,       // Pattern matching
    ARRAY,       // Array operations
    TYPECAST     // Type conversion (::)
};

// ---------------- Operators ----------------

/**
 * @enum ArithmeticOp
 * @brief Arithmetic operators.
 */
enum class ArithmeticOp {
    UNKNOWN = 0,
    PLUS,     // Addition
    MINUS,    // Subtraction
    MULTIPLY, // Multiplication
    DIVIDE,   // Division
    MOD       // Modulo
};

/**
 * @enum AssignOp
 * @brief Assignment operators.
 */
enum class AssignOp {
    UNKNOWN = 0,
    ASSIGN,       // =
    COLON_ASSIGN  // :=
};

/**
 * @enum ComparisonOp
 * @brief Comparison operators.
 */
enum class ComparisonOp {
    UNKNOWN = 0,
    // Basic comparisons
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    NOT_EQUAL, EQUAL,
    // Null-safe equality
    IS_DISTINCT_FROM, IS_NOT_DISTINCT_FROM,
    // Pattern matching
    LIKE, NOT_LIKE,
    ILIKE, NOT_ILIKE,
    SIMILAR_TO, NOT_SIMILAR_TO
};

/**
 * @enum LogicalOp
 * @brief Logical operators.
 */
enum class LogicalOp {
    UNKNOWN = 0,
    AND, OR, NOT,  // Basic logic
    XOR,           // Exclusive or
    IMPLIES        // Implication
};

/**
 * @enum BitwiseOp
 * @brief Bitwise operators.
 */
enum class BitwiseOp {
    UNKNOWN = 0,
    // Basic operations
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    // Shifts
    LEFT_SHIFT, RIGHT_SHIFT
};

/**
 * @enum ConcatOp
 * @brief String concatenation operators.
 */
enum class ConcatOp {
    UNKNOWN = 0,
    CONCAT  // ||
};

/**
 * @enum JsonOp
 * @brief JSON operators.
 */
enum class JsonOp {
    UNKNOWN = 0,
    // Path access
    ARROW, ARROW2,         // -> ->>
    HASH_ARROW, HASH_ARROW2, // #> #>>
    // Special operators
    AT,                    // @
    QUESTION,              // ?
    QUESTION_PIPE,         // ?|
    QUESTION_AMP,          // ?&
    HASH_MINUS            // #-
};

/**
 * @enum RegexOp
 * @brief Regular expression operators.
 */
enum class RegexOp {
    UNKNOWN = 0,
    TILDE,           // ~
    NOT_TILDE,       // !~
    TILDE_STAR,      // ~*
    NOT_TILDE_STAR   // !~*
};

enum class TypecastOp {
    UNKNOWN = 0,
    TYPECAST         // ::
};

// ---------------- Identifiers ----------------

/**
 * @enum IdentifierCategory
 * @brief Database object and identifier types.
 */
enum class IdentifierCategory {
    UNKNOWN = 0,
    // Schema objects
    TABLE, VIEW, PROCEDURE, FUNCTION, TRIGGER,
    INDEX, CONSTRAINT, SEQUENCE,
    // Database objects
    SCHEMA, DATABASE,
    // User objects
    USER_DEFINED_TYPE, ROLE, USER,
    // Special tables
    EXTERNAL_TABLE, TEMP_TABLE, GLOBAL_TEMP_TABLE,
    // Variables
    USER_VARIABLE, SYSTEM_VARIABLE,
    // Other identifiers
    COLUMN, PARAMETER, LABEL
};

// ---------------- Literals ----------------

/**
 * @enum LiteralCategory
 * @brief Literal value types.
 */
enum class LiteralCategory {
    UNKNOWN = 0,
    // String types
    STRING, ESCAPE_STRING, CHAR,
    // Numeric types
    INTEGER, FLOAT,
    // Binary types
    BINARY, HEX,
    // Date/Time types
    DATE, TIME, DATETIME, INTERVAL,
    // Special types
    UUID, ARRAY, JSON, XML,
    // Other types
    BOOLEAN, NULL_VALUE
};

// ---------------- Date/Time Components ----------------

/**
 * @enum DateTimePart
 * @brief Date/time component identifiers.
 */
enum class DateTimePart {
    UNKNOWN = 0,
    // Date parts
    YEAR, QUARTER, MONTH,
    DAY_OF_YEAR, DAY,
    WEEK, ISO_WEEK, WEEKDAY,
    // Time parts
    HOUR, MINUTE, SECOND,
    MILLISECOND, MICROSECOND, NANOSECOND,
    // Zone
    TIMEZONE_OFFSET
};

// ---------------- Delimiters and Punctuation ----------------

/**
 * @enum CommonSymbol
 * @brief Common SQL punctuation symbols.
 */
enum class CommonSymbol {
    UNKNOWN = 0,
    // Separators
    COMMA, SEMICOLON,
    // Grouping
    LPAREN, RPAREN,      // ()
    LBRACE, RBRACE,      // {}
    LBRACKET, RBRACKET,  // []
    // Others
    DOT, COLON,         // . :
    PARAM_MARKER        // ?
};

/**
 * @enum TSQLSymbol
 * @brief TSQL-specific symbols.
 */
enum class TSQLSymbol {
    UNKNOWN = 0,
    DOT,           // .
    COLON,         // :
};

/**
 * @enum StringDelimiter
 * @brief String delimiter types.
 */
enum class StringDelimiter {
    UNKNOWN = 0,
    SINGLE_QUOTE,  // '
    DOUBLE_QUOTE,  // "
    BACKTICK,      // `
    DOLLAR_QUOTE   // $$
};

// ---------------- Comments ----------------

/**
 * @enum CommentType
 * @brief SQL comment types.
 */
enum class CommentType {
    UNKNOWN = 0,
    SINGLE_LINE,  // --comment
    MULTI_LINE    // /*comment*/
};