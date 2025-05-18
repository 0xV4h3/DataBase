// File: Parser.hpp
// Purpose: Defines the Parser class, which turns a token vector into a navigable SQL AST tree.

#pragma once

#include "Token.hpp"
#include "AST.hpp"
#include "ASTNodes.hpp"
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <stdexcept>

/**
 * @brief SQL Parser: Converts a token stream into an Abstract Syntax Tree (AST).
 */
class Parser {
private:
    std::vector<Token> tokens;        ///< Token stream to parse.
    size_t pos = 0;                   ///< Current token position.
    std::unique_ptr<AST> ast;         ///< AST tree built after parsing.

    // === Navigation helpers ===

    const Token& currentToken() const;
    const Token& peek(int offset = 1) const;
    bool isAtEnd() const;
    void advance();
    bool match(TokenType type);
    bool match(TokenType type, const std::string& value);
    void expect(TokenType type, const std::string& errorMsg);
    void expect(TokenType type, const std::string& value, const std::string& errorMsg);

    [[noreturn]] void error(const std::string& message) const;

    // === Multi-token keyword helpers ===

    bool matchKeywordSeq(const std::vector<std::string>& keywords);

    // === Main parsing methods ===

    std::unique_ptr<ASTNode> parseQuery();
    std::unique_ptr<SelectStatementNode> parseSelectStatement();
    std::vector<std::unique_ptr<SelectItemNode>> parseSelectList();
    std::vector<std::unique_ptr<TableReferenceNode>> parseFromClause();
    std::vector<std::unique_ptr<JoinNode>> parseJoinClauses(std::vector<std::unique_ptr<TableReferenceNode>>& tables);
    std::unique_ptr<WhereNode> parseWhereClause();
    std::unique_ptr<GroupByNode> parseGroupByClause();
    std::unique_ptr<HavingNode> parseHavingClause();
    std::unique_ptr<OrderByNode> parseOrderByClause();
    std::unique_ptr<LimitNode> parseLimitClause();
    std::unique_ptr<ASTNode> parseSetOperation(std::unique_ptr<ASTNode> left);
    std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0);
    std::unique_ptr<SelectItemNode> parseSelectItem();
    std::unique_ptr<TableReferenceNode> parseTableReference();
    std::optional<std::string> parseOptionalAlias();

public:
    /**
     * @brief Construct a Parser given a list of tokens.
     */
    explicit Parser(const std::vector<Token>& tokens);

    /**
     * @brief Parses the token stream and builds the AST tree.
     */
    void parse();

    /**
     * @brief Access the built AST (const/non-const).
     */
    const AST* getAST() const;
    AST* getAST();
};