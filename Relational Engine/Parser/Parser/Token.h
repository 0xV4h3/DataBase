#pragma once

#include "literal.h"
#include <string>

enum class TokenType {
    KEYWORD, // SELECT, FROM, WHERE, INSERT, UPDATE, DELETE etc
    IDENTIFIER, // user created entities
    LITERAL, // const values
    OPERATOR, // +, -, *, /, %,  =, <, >, <=, >=, <>
    SYMBOL, // ','  , ';' , '()' , '{}', '[]'  
    END_OF_FILE, // input stream end
    UNKNOWN // unknown token
};

struct Token {
    TokenType type;
    std::string value;
    int position;
    LiteralType literalType = LiteralType::UNKNOWN;
};