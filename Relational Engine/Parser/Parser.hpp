#pragma once

#include "Token.hpp"
#include "AST.hpp"
#include "ASTNodes.hpp"
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <stdexcept>

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;
    std::unique_ptr<AST> ast; // Now the parser owns the AST

    // Navigation helpers
    const Token& currentToken() const;
    const Token& peek(int offset = 1) const;
    bool isAtEnd() const;
    void advance();
    bool match(TokenType type);
    bool match(TokenType type, const std::string& value); // for specific keyword/value
    void expect(TokenType type, const std::string& errorMsg);
    void expect(TokenType type, const std::string& value, const std::string& errorMsg);

    // Error reporting
    [[noreturn]] void error(const std::string& message) const;

    // Multi-token keyword helpers
    bool matchKeywordSeq(const std::vector<std::string>& keywords);

    // Top-level parsing
    std::unique_ptr<ASTNode> parseQuery(); // Handles SELECT, UNION, etc.

    // SELECT statement
    std::unique_ptr<SelectStatementNode> parseSelectStatement();

    // SELECT list (columns, expressions, *, etc.)
    std::vector<std::unique_ptr<SelectItemNode>> parseSelectList();

    // FROM clause and table sources (tables, subqueries)
    std::vector<std::unique_ptr<TableReferenceNode>> parseFromClause();

    // JOINs (INNER, LEFT OUTER, etc.)
    std::vector<std::unique_ptr<JoinNode>> parseJoinClauses(std::vector<std::unique_ptr<TableReferenceNode>>& tables);

    // WHERE clause
    std::unique_ptr<WhereNode> parseWhereClause();

    // GROUP BY clause
    std::unique_ptr<GroupByNode> parseGroupByClause();

    // HAVING clause
    std::unique_ptr<HavingNode> parseHavingClause();

    // ORDER BY clause
    std::unique_ptr<OrderByNode> parseOrderByClause();

    // LIMIT/OFFSET clause
    std::unique_ptr<LimitNode> parseLimitClause();

    // Set operations (UNION, INTERSECT, EXCEPT)
    std::unique_ptr<ASTNode> parseSetOperation(std::unique_ptr<ASTNode> left);

    // Expressions
    std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0);

    // Parse single select item (expression [AS alias])
    std::unique_ptr<SelectItemNode> parseSelectItem();

    // Table reference (table name, alias, or subquery)
    std::unique_ptr<TableReferenceNode> parseTableReference();

    // Utility: check for alias after table/column
    std::optional<std::string> parseOptionalAlias();

public:
    explicit Parser(const std::vector<Token>& tokens);

    // Parses and builds the AST tree
    void parse();

    // Access the built AST
    const AST* getAST() const;
    AST* getAST();
};