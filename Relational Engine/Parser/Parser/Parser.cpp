#include "Lexer.h"
#include <iostream>
#include <memory>
#include <string>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case TokenType::KEYWORD:      return "KEYWORD";
    case TokenType::IDENTIFIER:   return "IDENTIFIER";
    case TokenType::LITERAL:      return "LITERAL";
    case TokenType::OPERATOR:     return "OPERATOR";
    case TokenType::SYMBOL:       return "SYMBOL";
    case TokenType::END_OF_FILE:  return "END_OF_FILE";
    case TokenType::UNKNOWN:      return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string literalTypeToString(LiteralType type) {
    switch (type) {
    case LiteralType::STRING:     return "STRING";
    case LiteralType::CHAR:       return "CHAR";
    case LiteralType::INTEGER:    return "INTEGER";
    case LiteralType::FLOAT:      return "FLOAT";
    case LiteralType::BINARY:     return "BINARY";
    case LiteralType::HEX:        return "HEX";
    case LiteralType::DATE:       return "DATE";
    case LiteralType::TIME:       return "TIME";
    case LiteralType::DATETIME:   return "DATETIME";
    case LiteralType::JSON:       return "JSON";
    case LiteralType::BOOLEAN:    return "BOOLEAN";
    case LiteralType::NULL_VALUE: return "NULL_VALUE";
    case LiteralType::UNKNOWN:    return "UNKNOWN";
    }
    return "UNKNOWN";
}

int main() {
    std::string query =
        "SELECT \"Hello\", 'A', 123, -456, 3.14, B'1010', X'1A2F', "
        "'2025-03-30', '14:30:00', '2025-03-30 14:30:00', {\"key\": \"value\"}, TRUE, NULL FROM my_table;";

    Lexer lexer(query);

    std::unique_ptr<Token> token;
    std::cout << "Token list:\n";
    while ((token = lexer.nextToken())->type != TokenType::END_OF_FILE) {
        std::cout << "Token: \"" << token->value << "\""
            << " | Type: " << tokenTypeToString(token->type);
        if (token->type == TokenType::LITERAL) {
            std::cout << " (LiteralType: " << literalTypeToString(token->literalType) << ")";
        }
        std::cout << " | Position: " << token->position << std::endl;
    }
    std::cout << "Token: \"\" | Type: " << tokenTypeToString(token->type)
        << " | Position: " << token->position << std::endl;
    return 0;
}