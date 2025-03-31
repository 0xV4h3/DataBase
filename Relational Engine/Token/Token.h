#pragma once

#include <string>

enum class TokenType {
    KEYWORD, // SELECT, FROM, WHERE, INSERT, UPDATE, DELETE etc
    IDENTIFIER, // user created entities
    LITERAL, // const values
    OPERATOR, // +, -, *, /, %,  =, <, >, <=, >=, <>
    SYMBOL, // ','  , ';' , '()' , '{}', '[]'  
    END_OF_FILE, // input stream end
    UNKNOWN 
};

enum class Keyword {
    SELECT,
    FROM,
    WHERE,
    INSERT,
    UPDATE,
    DELETE,
    CREATE,
    DROP,
    ALTER,
    JOIN,
    ON,
    GROUP,
    BY,
    HAVING,
    ORDER,
    LIMIT,
    OFFSET,
    UNION,
    ALL,
    DISTINCT,
    AS,
    IN,
    IS,
    NULL_KEYWORD,
    TRUE_KEYWORD,
    FALSE_KEYWORD,
    UNKNOWN
};

enum class LiteralType {
    STRING,   // "..."
    CHAR,     // '...'
    INTEGER,  // 123, -456
    FLOAT,    // 3.14, -0.001
    BINARY,   // B'0101'
    HEX,       // X'01AF'
    DATE,     // '2025-03-30'
    TIME,     // '14:30:00'
    DATETIME, // '2025-03-30 14:30:00'
    JSON,     // '{ "key": "value" }'
    BOOLEAN,  // TRUE, FALSE
    NULL_VALUE, // NULL 
    UNKNOWN   
};

enum class Operator {
    PLUS,       // +
    MINUS,      // -
    MULTIPLY,   // *
    DIVIDE,     // /
    MOD,        // %
    ASSIGN,     // =
    LESS,       // <
    GREATER,    // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=
    NOT_EQUAL,     // <>
    UNKNOWN
};

enum class Symbol {
    COMMA,      // ,
    SEMICOLON,  // ;
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int position;
    LiteralType literalType = LiteralType::UNKNOWN;
};