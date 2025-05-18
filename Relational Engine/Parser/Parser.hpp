#pragma once

#include "Token.hpp"
#include "AST.hpp"
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    Token currentToken() const {
        return tokens[pos];
    }
    void advance() { pos++; }

public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}
};


