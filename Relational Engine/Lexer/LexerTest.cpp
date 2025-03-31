#include "Lexer.h"
#include <iostream>
#include <memory>
#include <string>

int main() {
    std::string query =
        "SELECT \"Hello\", 'A', 123, -456, 3.14, B'1010', X'1A2F', "
        "'2025-03-30', '14:30:00', '2025-03-30 14:30:00', {\"key\": \"value\"}, TRUE, NULL FROM my_table;";

    Lexer lexer(query);

    std::unique_ptr<Token> token;
    std::cout << "Token list:\n";
    while ((token = lexer.nextToken())->type != TokenType::END_OF_FILE) {
        std::cout << "Token: \"" << token->value << "\""
            << " | Type: " << Lexer::tokenTypeToString(token->type);
        if (token->type == TokenType::LITERAL) {
            std::cout << " (LiteralType: " << Lexer::literalTypeToString(token->literalType) << ")";
        }
        std::cout << " | Position: " << token->position << std::endl;
    }
    std::cout << "Token: \"\" | Type: " << Lexer::tokenTypeToString(token->type)
        << " | Position: " << token->position << std::endl;
    return 0;
}
