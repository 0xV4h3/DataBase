// File: ParserTest.cpp
// Purpose: Interactive integration tests for Lexer and Parser—tokenizes SQL queries, parses them to AST, and prints debug info.

#include "Lexer.hpp"
#include "LexerUtils.hpp"
#include "TokenUtils.hpp"
#include "Parser.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <typeinfo>
#include <string>
#include <limits>

// Demangle utility for class names (removes "class " or "struct " prefixes)
static std::string demangle(const char* name) {
    std::string s(name);
    if (s.find("class ") == 0) s = s.substr(6);
    if (s.find("struct ") == 0) s = s.substr(7);
    return s;
}

// Utility lambda: Print detailed token info in a readable format.
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
    // COMMENT
    else if (token.getType() == TokenType::COMMENT) {
        auto commentTok = dynamic_cast<const CommentToken*>(&token);
        if (commentTok) {
            std::cout << " (CommentType: " << LexerUtils::commentTypeToString(commentTok->commentType) << ")";
        }
    }

    std::cout << " | Position: " << token.getPosition() << std::endl;
    };

/**
 * @brief Print the type of each AST node using traverse.
 */
void print_ast_node_types(const AST* ast) {
    std::cout << "\nAST Node Types (traverse):\n";
    int nodeCount = 0;
    ast->traverse([&](ASTNode& node) {
        std::cout << "  [" << ++nodeCount << "] " << demangle(typeid(node).name()) << " : " << node.toString() << '\n';
        });
}

/**
 * @brief Run a SQL query through Lexer and Parser, print token info and AST.
 * @param query The SQL query string to test.
 * @param label Optional label for the test output.
 * @param printNodes If true, print AST node types using traverse.
 */
void run_query_test(const std::string& query, const std::string& label = "Query Test", bool printNodes = true) {
    std::cout << "\n=== " << label << " ===\n";
    // --- 1. Lexing phase: tokenize the input ---
    Lexer lexer(query);
    std::vector<Token> tokens;
    std::cout << "Token list:\n";
    while (true) {
        std::unique_ptr<Token> token = lexer.nextToken();
        print_token_info(*token);
        tokens.push_back(*token);
        if (token->getType() == TokenType::END_OF_FILE) break;
    }

    // --- 2. Parsing phase: build the AST from tokens ---
    try {
        Parser parser(tokens);
        parser.parse();
        const AST* ast = parser.getAST();
        if (ast) {
            // --- 3. Print the AST tree as a string ---
            std::cout << "\nAST toString():\n";
            std::cout << ast->toString() << std::endl;

            // --- 4. Traverse AST and print node types ---
            if (printNodes) print_ast_node_types(ast);

            // --- 5. Print the AST tree structure (pretty-print) ---
            std::cout << "\nAST tree structure:\n";
            ast->print(std::cout);
        }
        else {
            std::cout << "Parser produced no AST.\n";
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Exception during parsing: " << ex.what() << std::endl;
    }

    std::cout << std::string(60, '-') << std::endl;
}

int get_valid_choice(int min_choice, int max_choice) {
    int choice;
    while (true) {
        std::cout << "Enter your choice: ";
        std::string input;
        std::getline(std::cin, input);

        // Check if input is an integer
        try {
            size_t idx;
            choice = std::stoi(input, &idx);
            if (idx != input.size()) throw std::invalid_argument("Extra chars");
        }
        catch (...) {
            std::cout << "Invalid input. Please enter a number between " << min_choice << " and " << max_choice << ".\n";
            continue;
        }
        if (choice < min_choice || choice > max_choice) {
            std::cout << "Choice out of range. Please enter a number between " << min_choice << " and " << max_choice << ".\n";
            continue;
        }
        return choice;
    }
}

std::string get_multiline_sql() {
    std::cout << "\nEnter your SQL query (multi-line). Type 'end' or 'END' on a new line to finish:\n";
    std::string line, full_query;
    while (true) {
        std::getline(std::cin, line);
        if (line == "end" || line == "END") break;
        full_query += line + "\n";
    }
    return full_query;
}

void print_menu() {
    std::cout << "\n=== SQL Parser Test Menu ===\n";
    std::cout << "0. Input your own SQL query (multi-line, end with ;)\n";
    std::cout << "1. Advanced Query Test\n";
    std::cout << "2. Simple SELECT Test\n";
    std::cout << "3. Set Operations Test\n";
    std::cout << "4. Subquery Test\n";
    std::cout << "5. All Joins Test\n";
    std::cout << "6. Nested SELECTs Test\n";
    std::cout << "7. Malformed Query (Missing FROM)\n";
    std::cout << "8. Malformed Query (Bad Parentheses)\n";
    std::cout << "9. Malformed Query (Incomplete JOIN)\n";
    std::cout << "10. Malformed Query (Invalid Keyword Usage)\n";
    std::cout << "11. Malformed Query (Stray Comma)\n";
    std::cout << "12. Exit\n";
}

int main() {
    constexpr int min_choice = 0, max_choice = 12;
    while (true) {
        print_menu();
        int choice = get_valid_choice(min_choice, max_choice);

        if (choice == 12) {
            std::cout << "\nTest is over. Goodbye!\n";
            break;
        }

        switch (choice) {
        case 0: {
            std::string user_query = get_multiline_sql();
            run_query_test(user_query, "User Input Query");
            break;
        }
        case 1: {
            const std::string advanced_query =
                "SELECT DISTINCT TOP 10 id, name AS username, SUM(balance) AS total "
                "FROM accounts a "
                "LEFT JOIN transactions t ON a.id = t.account_id "
                "WHERE a.active = 1 AND t.amount > 0 "
                "GROUP BY id, name "
                "HAVING SUM(balance) > 1000 "
                "ORDER BY total DESC "
                "LIMIT 10 OFFSET 5;";
            run_query_test(advanced_query, "Advanced Query Test");
            break;
        }
        case 2: {
            const std::string simple_query = "SELECT * FROM users;";
            run_query_test(simple_query, "Simple SELECT Test");
            break;
        }
        case 3: {
            const std::string setop_query =
                "SELECT id FROM t1 "
                "UNION ALL "
                "SELECT id FROM t2 "
                "INTERSECT "
                "SELECT id FROM t3;";
            run_query_test(setop_query, "Set Operations Test");
            break;
        }
        case 4: {
            const std::string subquery_test =
                "SELECT name, (SELECT COUNT(*) FROM orders WHERE orders.user_id = users.id) AS order_count "
                "FROM users "
                "WHERE EXISTS (SELECT 1 FROM orders WHERE orders.user_id = users.id);";
            run_query_test(subquery_test, "Subquery Test");
            break;
        }
        case 5: {
            const std::string all_joins_test =
                "SELECT * FROM a "
                "LEFT JOIN b ON a.id = b.aid "
                "RIGHT JOIN c ON a.id = c.aid "
                "FULL JOIN d ON a.id = d.aid "
                "INNER JOIN e ON a.id = e.aid;";
            run_query_test(all_joins_test, "All Joins Test");
            break;
        }
        case 6: {
            const std::string nested_selects_test =
                "SELECT id, (SELECT AVG(salary) FROM employees WHERE department_id = d.id) AS avg_salary "
                "FROM departments d "
                "WHERE d.budget > (SELECT SUM(salary) FROM employees WHERE department_id = d.id);";
            run_query_test(nested_selects_test, "Nested SELECTs Test");
            break;
        }
        case 7: {
            const std::string malformed_missing_from = "SELECT id name;";
            run_query_test(malformed_missing_from, "Malformed Query (Missing FROM)");
            break;
        }
        case 8: {
            const std::string malformed_paren = "SELECT (id, name FROM users;";
            run_query_test(malformed_paren, "Malformed Query (Bad Parentheses)");
            break;
        }
        case 9: {
            const std::string malformed_join = "SELECT * FROM a LEFT JOIN b;";
            run_query_test(malformed_join, "Malformed Query (Incomplete JOIN)");
            break;
        }
        case 10: {
            const std::string malformed_keyword = "SELECT FROM users WHERE;";
            run_query_test(malformed_keyword, "Malformed Query (Invalid Keyword Usage)");
            break;
        }
        case 11: {
            const std::string malformed_comma = "SELECT id, FROM users;";
            run_query_test(malformed_comma, "Malformed Query (Stray Comma)");
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
