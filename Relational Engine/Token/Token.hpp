#pragma once

#include <string>
#include <utility>

/**
 * @brief Types of tokens produced by the lexer.
 */
enum class TokenType {
    KEYWORD,
    FUNCTION,
    IDENTIFIER,
    LITERAL,
    OPERATOR,
    PUNCTUATOR,
    DATETIMEPART,
    COMMENT,
    END_OF_FILE,
    UNKNOWN
};

// ---------------- Keyword categories and their enums ----------------

enum class DMLKeyword { SELECT, INSERT, UPDATE, DELETE, MERGE, EXECUTE, UNKNOWN };
enum class DDLKeyword { CREATE, ALTER, DROP, TRUNCATE, UNKNOWN };
enum class ClauseKeyword { FROM, WHERE, JOIN, ON, GROUP, BY, HAVING, ORDER, UNION, ALL, DISTINCT, TOP, LIMIT, OFFSET, LEFT, RIGHT, FULL, OUTER, INNER, CROSS, APPLY, UNKNOWN };
enum class CTEKeyword { WITH, UNKNOWN };
enum class SetOpKeyword { EXCEPT, INTERSECT, UNKNOWN };
enum class WordOperatorKeyword { IN, IS, NOT, LIKE, BETWEEN, UNKNOWN };
enum class LogicalConstantKeyword { NULL_KEYWORD, TRUE_KEYWORD, FALSE_KEYWORD, UNKNOWN };
enum class TransactionKeyword { BEGIN, COMMIT, ROLLBACK, SAVEPOINT, UNKNOWN };
enum class SecurityKeyword { GRANT, REVOKE, DENY, UNKNOWN };
enum class ProgStmtKeyword { DECLARE, SET, PRINT, RETURN, THROW, TRY, CATCH, UNKNOWN };
enum class MiscKeyword { AS, CASE, WHEN, THEN, ELSE, END, CAST, CONVERT, ASC, DESC, UNKNOWN };

// ---------------- Function categories and their enums ----------------

enum class AggregateFunction { COUNT, SUM, AVG, MIN, MAX, UNKNOWN };
enum class ScalarFunction { CONVERT, CAST, COALESCE, LENGTH, UNKNOWN };
enum class StringFunction { UPPER, LOWER, SUBSTRING, TRIM, UNKNOWN };
enum class DateTimeFunction { DATEPART, GETDATE, NOW, UNKNOWN };
enum class MathFunction { ABS, CEILING, FLOOR, ROUND, UNKNOWN };
enum class SystemFunction { SUSER_SNAME, CURRENT_USER, TRANCOUNT, UNKNOWN };

// ---------------- Category enums for token types ----------------

enum class KeywordCategory {
    DML, DDL, CLAUSE, CTE, SETOP, WORD_OP, LOGICAL_CONST,
    TRANSACTION, SECURITY, PROG_STMT, MISC, UNKNOWN
};
enum class FunctionCategory {
    AGGREGATE, SCALAR, STRING, DATETIME, MATHEMATICAL, SYSTEM, UNKNOWN
};
enum class OperatorCategory {
    ARITHMETIC, ASSIGN, COMPARISON, LOGICAL, BITWISE, CONCAT, UNKNOWN
};
enum class PunctuatorCategory {
    COMMON, TSQL, STRING_DELIM, UNKNOWN
};

// ---------------- Identifier category enums ----------------

enum class IdentifierCategory {
    TABLE, VIEW, PROCEDURE, FUNCTION, TRIGGER, INDEX, CONSTRAINT,
    SCHEMA, DATABASE, SEQUENCE, USER_DEFINED_TYPE, ROLE, USER, EXTERNAL_TABLE,
    USER_VARIABLE, SYSTEM_VARIABLE, TEMP_TABLE, GLOBAL_TEMP_TABLE,
    UNKNOWN
};

// ---------------- Literal category enums ----------------

enum class LiteralCategory {
    STRING, CHAR, INTEGER, FLOAT, BINARY, HEX,
    DATE, TIME, DATETIME, JSON, BOOLEAN, NULL_VALUE, UNKNOWN
};

// ---------------- Operator enums ----------------

enum class ArithmeticOp { PLUS, MINUS, MULTIPLY, DIVIDE, MOD, UNKNOWN };
enum class AssignOp { ASSIGN, UNKNOWN };
enum class ComparisonOp { LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, NOT_EQUAL, EQUAL, UNKNOWN };
enum class LogicalOp { AND, OR, NOT, UNKNOWN };
enum class BitwiseOp { BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT, LEFT_SHIFT, RIGHT_SHIFT, UNKNOWN };
enum class ConcatOp { CONCAT, UNKNOWN };

// ---------------- Date/Time part identifiers (for DATEPART) ----------------

enum class DateTimePart {
    YEAR, QUARTER, MONTH, DAY_OF_YEAR, DAY, WEEK, ISO_WEEK, WEEKDAY,
    HOUR, MINUTE, SECOND, MILLISECOND, MICROSECOND, NANOSECOND, TIMEZONE_OFFSET, UNKNOWN
};

// ---------------- Punctuator and delimiter enums ----------------

enum class CommonSymbol { COMMA, SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, UNKNOWN };
enum class TSQLSymbol { DOT, COLON, DOUBLE_COLON, UNKNOWN };
enum class StringDelimiter { SINGLE_QUOTE, DOUBLE_QUOTE, BACKTICK, UNKNOWN };

// ---------------- Comment category enum ----------------

enum class CommentType { SINGLE_LINE, MULTI_LINE };

// ---------------- Base Token class and derived token classes ----------------

typedef TokenType TT;

/**
 * @brief Base class for all tokens.
 */
class Token {
public:
    Token() : type_(TT::UNKNOWN), value_(), position_(-1) {}
    Token(TT t, std::string v, int p) : type_(t), value_(std::move(v)), position_(p) {}
    Token(TT t, std::string v) : type_(t), value_(std::move(v)), position_(-1) {}
    virtual ~Token() = default;

    TT getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    int getPosition() const { return position_; }

    void setPosition(int position) { position_ = (position >= 0 ? position : -1); }

protected:
    TT type_;
    std::string value_;
    int position_;
};

/**
 * @brief Specialized token for SQL keywords.
 */
class KeywordToken : public Token {
public:
    KeywordCategory category;
    union {
        DMLKeyword dml;
        DDLKeyword ddl;
        ClauseKeyword clause;
        CTEKeyword cte;
        SetOpKeyword setop;
        WordOperatorKeyword wordop;
        LogicalConstantKeyword lconst;
        TransactionKeyword tran;
        SecurityKeyword sec;
        ProgStmtKeyword prog;
        MiscKeyword misc;
    } kind;

    KeywordToken(DMLKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::DML) {
        kind.dml = k;
    }
    KeywordToken(DDLKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::DDL) {
        kind.ddl = k;
    }
    KeywordToken(ClauseKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::CLAUSE) {
        kind.clause = k;
    }
    KeywordToken(CTEKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::CTE) {
        kind.cte = k;
    }
    KeywordToken(SetOpKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::SETOP) {
        kind.setop = k;
    }
    KeywordToken(WordOperatorKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::WORD_OP) {
        kind.wordop = k;
    }
    KeywordToken(LogicalConstantKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::LOGICAL_CONST) {
        kind.lconst = k;
    }
    KeywordToken(TransactionKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::TRANSACTION) {
        kind.tran = k;
    }
    KeywordToken(SecurityKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::SECURITY) {
        kind.sec = k;
    }
    KeywordToken(ProgStmtKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::PROG_STMT) {
        kind.prog = k;
    }
    KeywordToken(MiscKeyword k, std::string v)
        : Token(TT::KEYWORD, std::move(v)), category(KeywordCategory::MISC) {
        kind.misc = k;
    }
    KeywordToken() : Token(TT::KEYWORD, "", -1), category(KeywordCategory::UNKNOWN) { kind.misc = MiscKeyword::UNKNOWN; }
};

/**
 * @brief Specialized token for SQL functions.
 */
class FunctionToken : public Token {
public:
    FunctionCategory category;
    union {
        AggregateFunction agg;
        ScalarFunction scal;
        StringFunction str;
        DateTimeFunction dt;
        MathFunction math;
        SystemFunction sys;
    } func;

    FunctionToken(AggregateFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::AGGREGATE) {
        func.agg = f;
    }
    FunctionToken(ScalarFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::SCALAR) {
        func.scal = f;
    }
    FunctionToken(StringFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::STRING) {
        func.str = f;
    }
    FunctionToken(DateTimeFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::DATETIME) {
        func.dt = f;
    }
    FunctionToken(MathFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::MATHEMATICAL) {
        func.math = f;
    }
    FunctionToken(SystemFunction f, std::string v)
        : Token(TT::FUNCTION, std::move(v)), category(FunctionCategory::SYSTEM) {
        func.sys = f;
    }
    FunctionToken() : Token(TT::FUNCTION, "", -1), category(FunctionCategory::UNKNOWN) { func.agg = AggregateFunction::UNKNOWN; }
};

/**
 * @brief Token for SQL identifiers.
 */
class IdentifierToken : public Token {
public:
    IdentifierCategory idType;
    IdentifierToken(IdentifierCategory it, std::string v)
        : Token(TT::IDENTIFIER, std::move(v)), idType(it) {
    }
};

/**
 * @brief Token for SQL literals.
 */
class LiteralToken : public Token {
public:
    LiteralCategory litType;
    LiteralToken(LiteralCategory lt, std::string v)
        : Token(TT::LITERAL, std::move(v)), litType(lt) {
    }
};

/**
 * @brief Token for SQL operators.
 */
class OperatorToken : public Token {
public:
    OperatorCategory category;
    union {
        ArithmeticOp ar;
        AssignOp as;
        ComparisonOp comp;
        LogicalOp log;
        BitwiseOp bit;
        ConcatOp concat;
    } op;

    OperatorToken(ArithmeticOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::ARITHMETIC) {
        op.ar = o;
    }
    OperatorToken(AssignOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::ASSIGN) {
        op.as = o;
    }
    OperatorToken(ComparisonOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::COMPARISON) {
        op.comp = o;
    }
    OperatorToken(LogicalOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::LOGICAL) {
        op.log = o;
    }
    OperatorToken(BitwiseOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::BITWISE) {
        op.bit = o;
    }
    OperatorToken(ConcatOp o, std::string v)
        : Token(TT::OPERATOR, std::move(v)), category(OperatorCategory::CONCAT) {
        op.concat = o;
    }
    OperatorToken() : Token(TT::OPERATOR, "", -1), category(OperatorCategory::UNKNOWN) { op.ar = ArithmeticOp::UNKNOWN; }
};

/**
 * @brief Token for SQL date/time part (e.g. YEAR, MONTH).
 */
class DateTimePartToken : public Token {
public:
    DateTimePart part;
    DateTimePartToken(DateTimePart p, std::string v)
        : Token(TT::DATETIMEPART, std::move(v)), part(p) {
    }
    DateTimePart getPart() const { return part; }
};

/**
 * @brief Token for SQL punctuators (symbols, delimiters).
 */
class PunctuatorToken : public Token {
public:
    PunctuatorCategory category;
    union {
        CommonSymbol cs;
        TSQLSymbol ts;
        StringDelimiter sd;
    } sym;

    PunctuatorToken(CommonSymbol s, std::string v)
        : Token(TT::PUNCTUATOR, std::move(v)), category(PunctuatorCategory::COMMON) {
        sym.cs = s;
    }
    PunctuatorToken(TSQLSymbol s, std::string v)
        : Token(TT::PUNCTUATOR, std::move(v)), category(PunctuatorCategory::TSQL) {
        sym.ts = s;
    }
    PunctuatorToken(StringDelimiter s, std::string v)
        : Token(TT::PUNCTUATOR, std::move(v)), category(PunctuatorCategory::STRING_DELIM) {
        sym.sd = s;
    }
    PunctuatorToken() : Token(TT::PUNCTUATOR, "", -1), category(PunctuatorCategory::UNKNOWN) { sym.cs = CommonSymbol::UNKNOWN; }
};

/**
 * @brief Token for SQL comments.
 */
class CommentToken : public Token {
public:
    CommentType commentType;
    CommentToken(CommentType t, std::string v)
        : Token(TT::COMMENT, std::move(v)), commentType(t) {
    }
};