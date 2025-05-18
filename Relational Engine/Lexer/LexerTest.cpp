#include "Lexer.hpp"
#include "LexerUtils.hpp"
#include "TokenUtils.hpp"
#include <iostream>
#include <memory>
#include <string>

int main() {
    std::string advanced_query =
        "SELECT\n"
        "    C.CustomerID,\n"
        "    C.FirstName,\n"
        "    C.LastName,\n"
        "    O.OrderID,\n"
        "    O.TotalAmount,\n"
        "    DATEPART(YEAR, O.OrderDate) AS OrderYear,\n"
        "    DATEPART(MONTH, O.OrderDate) AS OrderMonth,\n"
        "    A.City,\n"
        "    A.State,\n"
        "    CASE \n"
        "        WHEN O.TotalAmount >= 500 THEN 'VIP'\n"
        "        ELSE 'Standard'\n"
        "    END AS CustomerType\n"
        "FROM\n"
        "    Customers C\n"
        "    INNER JOIN Orders O ON C.CustomerID = O.CustomerID\n"
        "    LEFT JOIN Addresses A ON C.AddressID = A.AddressID\n"
        "WHERE\n"
        "    O.Status = 'Completed'\n"
        "    AND DATEPART(YEAR, O.OrderDate) = 2024\n"
        "ORDER BY\n"
        "    O.TotalAmount DESC,\n"
        "    C.LastName ASC;";

    std::string all_literals_query =
        "SELECT "
        "\"String\", 'C', 123, -456, 3.14, B'1010', X'1A2F', "
        "'2025-03-30', '14:30:00', '2025-03-30 14:30:00', "
        "{\"key\": \"value\"}, TRUE, FALSE, NULL "
        "FROM test_table;";

    auto print_token_info = [](const Token& token) {
        std::cout << "Token: \"" << token.getValue() << "\""
            << " | Type: " << LexerUtils::tokenTypeToString(token.getType());

        // DATETIMEPART
        if (token.getType() == TokenType::DATETIMEPART) {
            auto dtpart = dynamic_cast<const DateTimePartToken*>(&token);
            if (dtpart) {
                std::cout << " (DateTimePart: " << LexerUtils::DateTimePartTypeToString(dtpart->getPart()) << ")";
            }
        }
        // LITERAL
        else if (token.getType() == TokenType::LITERAL) {
            auto lit = dynamic_cast<const LiteralToken*>(&token);
            if (lit) {
                std::cout << " (LiteralType: " << LexerUtils::literalTypeToString(lit->litType) << ")";
            }
        }
        // IDENTIFIER
        else if (token.getType() == TokenType::IDENTIFIER) {
            auto id = dynamic_cast<const IdentifierToken*>(&token);
            if (id) {
                std::cout << " (IdentifierType: " << LexerUtils::identifierTypeToString(id->idType) << ")";
            }
        }
        // KEYWORD
        else if (token.getType() == TokenType::KEYWORD) {
            auto kw = dynamic_cast<const KeywordToken*>(&token);
            if (kw) {
                std::cout << " (KeywordClass: " << TokenUtils::getKeywordClassName(*kw)
                    << ", Keyword: " << TokenUtils::getKeywordName(*kw) << ")";
            }
        }
        // FUNCTION
        else if (token.getType() == TokenType::FUNCTION) {
            auto func = dynamic_cast<const FunctionToken*>(&token);
            if (func) {
                std::cout << " (FunctionClass: " << TokenUtils::getFunctionClassName(*func)
                    << ", Function: " << TokenUtils::getFunctionName(*func) << ")";
            }
        }
        // OPERATOR
        else if (token.getType() == TokenType::OPERATOR) {
            auto op = dynamic_cast<const OperatorToken*>(&token);
            if (op) {
                std::cout << " (OperatorClass: " << TokenUtils::getOperatorClassName(*op)
                    << ", Operator: " << TokenUtils::getOperatorName(*op) << ")";
            }
        }
        // PUNCTUATOR
        else if (token.getType() == TokenType::PUNCTUATOR) {
            auto punc = dynamic_cast<const PunctuatorToken*>(&token);
            if (punc) {
                std::cout << " (PunctuatorClass: " << TokenUtils::getPunctuatorClassName(*punc)
                    << ", Punctuator: " << TokenUtils::getPunctuatorName(*punc) << ")";
            }
        }

        std::cout << " | Position: " << token.getPosition() << std::endl;
        };

    // Test 1: Advanced Query
    std::cout << "=== Advanced Query Test ===" << std::endl;
    {
        Lexer lexer(advanced_query);
        std::unique_ptr<Token> token;
        std::cout << "Token list:\n";
        while ((token = lexer.nextToken())->getType() != TokenType::END_OF_FILE) {
            print_token_info(*token);
        }
        // EOF Token
        print_token_info(*token);
    }

    // Test 2: All Literals Query
    std::cout << "\n=== All Literals Query Test ===" << std::endl;
    {
        Lexer lexer(all_literals_query);
        std::unique_ptr<Token> token;
        std::cout << "Token list:\n";
        while ((token = lexer.nextToken())->getType() != TokenType::END_OF_FILE) {
            print_token_info(*token);
        }
        // EOF Token
        print_token_info(*token);
    }

    return 0;
}