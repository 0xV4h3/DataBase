// File: Parser.cpp
// Purpose: Implements the SQL Parser, building an AST from a vector of tokens.

#include "Parser.hpp"
#include <sstream>
#include <cctype>

// === Construction ===

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0), ast(nullptr) {
}

// === Navigation helpers ===

const Token& Parser::currentToken() const {
    if (pos < tokens.size())
        return tokens[pos];
    throw std::out_of_range("Parser::currentToken: No more tokens");
}

const Token& Parser::peek(int offset) const {
    size_t target = pos + offset;
    if (target < tokens.size())
        return tokens[target];
    throw std::out_of_range("Parser::peek: Peeking past end of tokens");
}

bool Parser::isAtEnd() const {
    return pos >= tokens.size() || tokens[pos].getType() == TokenType::END_OF_FILE;
}

void Parser::advance() {
    if (!isAtEnd())
        ++pos;
}

bool Parser::match(TokenType type) {
    if (!isAtEnd() && currentToken().getType() == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(TokenType type, const std::string& value) {
    if (!isAtEnd() &&
        currentToken().getType() == type &&
        currentToken().getValue() == value) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& errorMsg) {
    if (!match(type))
        error(errorMsg);
}

void Parser::expect(TokenType type, const std::string& value, const std::string& errorMsg) {
    if (!match(type, value))
        error(errorMsg);
}

[[noreturn]] void Parser::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "Parse error at token "
        << pos << " [value: '" << (isAtEnd() ? "<EOF>" : currentToken().getValue())
        << "']: " << message;
    throw std::runtime_error(oss.str());
}

// === Multi-token keyword helpers ===

bool Parser::matchKeywordSeq(const std::vector<std::string>& keywords) {
    size_t tempPos = pos;
    for (const auto& kw : keywords) {
        if (tempPos >= tokens.size() ||
            tokens[tempPos].getType() != TokenType::KEYWORD ||
            tokens[tempPos].getValue() != kw)
            return false;
        ++tempPos;
    }
    pos = tempPos;
    return true;
}

// === Main entry point ===

void Parser::parse() {
    std::unique_ptr<ASTNode> root = parseQuery();
    ast = std::make_unique<AST>(std::move(root));
}

const AST* Parser::getAST() const {
    return ast.get();
}

AST* Parser::getAST() {
    return ast.get();
}

// === Top-level parsing ===

std::unique_ptr<ASTNode> Parser::parseQuery() {
    // Handle SELECT and set operations (UNION, INTERSECT, EXCEPT)
    if (match(TokenType::KEYWORD, "SELECT")) {
        auto selectNode = parseSelectStatement();
        // Check for set operations (UNION, INTERSECT, EXCEPT)
        return parseSetOperation(std::move(selectNode));
    }
    error("Unsupported statement: expected SELECT.");
}

std::unique_ptr<ASTNode> Parser::parseSetOperation(std::unique_ptr<ASTNode> left) {
    // Supports chaining: SELECT ... UNION SELECT ... INTERSECT SELECT ...
    while (true) {
        if (matchKeywordSeq({ "UNION" })) {
            bool all = match(TokenType::KEYWORD, "ALL");
            expect(TokenType::KEYWORD, "SELECT", "Expected SELECT after UNION");
            auto right = parseSelectStatement();
            left = std::make_unique<SetOperationNode>(SetOperationType::UNION, all, std::move(left), std::move(right));
        }
        else if (matchKeywordSeq({ "INTERSECT" })) {
            bool all = match(TokenType::KEYWORD, "ALL");
            expect(TokenType::KEYWORD, "SELECT", "Expected SELECT after INTERSECT");
            auto right = parseSelectStatement();
            left = std::make_unique<SetOperationNode>(SetOperationType::INTERSECT, all, std::move(left), std::move(right));
        }
        else if (matchKeywordSeq({ "EXCEPT" })) {
            bool all = match(TokenType::KEYWORD, "ALL");
            expect(TokenType::KEYWORD, "SELECT", "Expected SELECT after EXCEPT");
            auto right = parseSelectStatement();
            left = std::make_unique<SetOperationNode>(SetOperationType::EXCEPT, all, std::move(left), std::move(right));
        }
        else {
            break;
        }
    }
    return std::make_unique<QueryRootNode>(std::move(left));
}

// === SELECT statement and clauses parsing ===

std::unique_ptr<SelectStatementNode> Parser::parseSelectStatement() {
    auto node = std::make_unique<SelectStatementNode>();

    if (match(TokenType::KEYWORD, "DISTINCT"))
        node->distinct = true;

    if (match(TokenType::KEYWORD, "TOP")) {
        if (currentToken().getType() == TokenType::LITERAL) {
            node->topN = std::stoi(currentToken().getValue());
            advance();
        }
        else {
            error("Expected number after TOP.");
        }
    }

    node->selectItems = parseSelectList();

    if (match(TokenType::KEYWORD, "FROM")) {
        node->from = parseFromClause();
        node->joins = parseJoinClauses(node->from);
    }

    node->where = parseWhereClause();
    node->groupBy = parseGroupByClause();
    node->having = parseHavingClause();
    node->orderBy = parseOrderByClause();
    node->limit = parseLimitClause();

    return node;
}

std::vector<std::unique_ptr<SelectItemNode>> Parser::parseSelectList() {
    std::vector<std::unique_ptr<SelectItemNode>> items;
    do {
        items.push_back(parseSelectItem());
    } while (match(TokenType::PUNCTUATOR, ","));
    return items;
}

std::unique_ptr<SelectItemNode> Parser::parseSelectItem() {
    auto expr = parseExpression();
    std::optional<std::string> alias;
    if (match(TokenType::KEYWORD, "AS")) {
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            alias = currentToken().getValue();
            advance();
        }
        else {
            error("Expected identifier after AS.");
        }
    }
    else if (currentToken().getType() == TokenType::IDENTIFIER) {
        alias = currentToken().getValue();
        advance();
    }
    return std::make_unique<SelectItemNode>(std::move(expr), alias);
}

std::vector<std::unique_ptr<TableReferenceNode>> Parser::parseFromClause() {
    std::vector<std::unique_ptr<TableReferenceNode>> tables;
    do {
        tables.push_back(parseTableReference());
    } while (match(TokenType::PUNCTUATOR, ","));
    return tables;
}

std::unique_ptr<TableReferenceNode> Parser::parseTableReference() {
    if (match(TokenType::PUNCTUATOR, "(")) {
        auto subquery = parseQuery();
        expect(TokenType::PUNCTUATOR, ")", "Expected ) after subquery in FROM");
        auto alias = parseOptionalAlias();
        return std::make_unique<TableReferenceNode>(std::move(subquery), alias);
    }
    else if (currentToken().getType() == TokenType::IDENTIFIER) {
        std::string tableName = currentToken().getValue();
        advance();
        auto alias = parseOptionalAlias();
        return std::make_unique<TableReferenceNode>(tableName, alias);
    }
    else {
        error("Expected table name or subquery in FROM clause.");
    }
}

std::optional<std::string> Parser::parseOptionalAlias() {
    if (match(TokenType::KEYWORD, "AS")) {
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            std::string alias = currentToken().getValue();
            advance();
            return alias;
        }
        else {
            error("Expected alias identifier after AS.");
        }
    }
    else if (currentToken().getType() == TokenType::IDENTIFIER) {
        std::string alias = currentToken().getValue();
        advance();
        return alias;
    }
    return std::nullopt;
}

std::vector<std::unique_ptr<JoinNode>> Parser::parseJoinClauses(std::vector<std::unique_ptr<TableReferenceNode>>& tables) {
    std::vector<std::unique_ptr<JoinNode>> joins;
    while (true) {
        std::string joinType;
        if (matchKeywordSeq({ "INNER", "JOIN" })) {
            joinType = "INNER";
        }
        else if (matchKeywordSeq({ "LEFT", "OUTER", "JOIN" })) {
            joinType = "LEFT OUTER";
        }
        else if (matchKeywordSeq({ "LEFT", "JOIN" })) {
            joinType = "LEFT";
        }
        else if (matchKeywordSeq({ "RIGHT", "OUTER", "JOIN" })) {
            joinType = "RIGHT OUTER";
        }
        else if (matchKeywordSeq({ "RIGHT", "JOIN" })) {
            joinType = "RIGHT";
        }
        else if (matchKeywordSeq({ "FULL", "OUTER", "JOIN" })) {
            joinType = "FULL OUTER";
        }
        else if (matchKeywordSeq({ "FULL", "JOIN" })) {
            joinType = "FULL";
        }
        else if (matchKeywordSeq({ "CROSS", "JOIN" })) {
            joinType = "CROSS";
        }
        else if (matchKeywordSeq({ "JOIN" })) {
            joinType = "INNER";
        }
        else {
            break;
        }

        auto right = parseTableReference();

        expect(TokenType::KEYWORD, "ON", "Expected ON after JOIN");
        auto onExpr = parseExpression();

        if (tables.empty())
            error("No table to join from (JOIN clause)");
        auto left = std::move(tables.back());
        tables.pop_back();
        joins.push_back(std::make_unique<JoinNode>(joinType, std::move(left), std::move(right), std::move(onExpr)));
        tables.push_back(std::make_unique<TableReferenceNode>(joins.back()->toString(), std::nullopt));
    }
    return joins;
}

std::unique_ptr<WhereNode> Parser::parseWhereClause() {
    if (match(TokenType::KEYWORD, "WHERE")) {
        auto cond = parseExpression();
        return std::make_unique<WhereNode>(std::move(cond));
    }
    return nullptr;
}

std::unique_ptr<GroupByNode> Parser::parseGroupByClause() {
    if (matchKeywordSeq({ "GROUP", "BY" })) {
        std::vector<std::unique_ptr<ExpressionNode>> groupExprs;
        do {
            groupExprs.push_back(parseExpression());
        } while (match(TokenType::PUNCTUATOR, ","));
        auto node = std::make_unique<GroupByNode>();
        node->groupExprs = std::move(groupExprs);
        return node;
    }
    return nullptr;
}

std::unique_ptr<HavingNode> Parser::parseHavingClause() {
    if (match(TokenType::KEYWORD, "HAVING")) {
        auto cond = parseExpression();
        return std::make_unique<HavingNode>(std::move(cond));
    }
    return nullptr;
}

std::unique_ptr<OrderByNode> Parser::parseOrderByClause() {
    if (matchKeywordSeq({ "ORDER", "BY" })) {
        auto node = std::make_unique<OrderByNode>();
        do {
            OrderByNode::OrderItem item;
            item.expr = parseExpression();
            if (match(TokenType::KEYWORD, "ASC")) {
                item.ascending = true;
            }
            else if (match(TokenType::KEYWORD, "DESC")) {
                item.ascending = false;
            }
            else {
                item.ascending = true;
            }
            node->orderItems.push_back(std::move(item));
        } while (match(TokenType::PUNCTUATOR, ","));
        return node;
    }
    return nullptr;
}

std::unique_ptr<LimitNode> Parser::parseLimitClause() {
    if (match(TokenType::KEYWORD, "LIMIT")) {
        int limit = 0, offset = 0;
        if (currentToken().getType() == TokenType::LITERAL) {
            limit = std::stoi(currentToken().getValue());
            advance();
        }
        else {
            error("Expected number after LIMIT.");
        }
        if (match(TokenType::PUNCTUATOR, ",")) {
            if (currentToken().getType() == TokenType::LITERAL) {
                offset = limit;
                limit = std::stoi(currentToken().getValue());
                advance();
            }
            else {
                error("Expected number after comma in LIMIT.");
            }
        }
        return std::make_unique<LimitNode>(limit, offset);
    }
    else if (match(TokenType::KEYWORD, "OFFSET")) {
        int offset = 0, limit = 0;
        if (currentToken().getType() == TokenType::LITERAL) {
            offset = std::stoi(currentToken().getValue());
            advance();
        }
        else {
            error("Expected number after OFFSET.");
        }
        if (matchKeywordSeq({ "ROWS", "FETCH", "NEXT" })) {
            if (currentToken().getType() == TokenType::LITERAL) {
                limit = std::stoi(currentToken().getValue());
                advance();
            }
            else {
                error("Expected number after FETCH NEXT.");
            }
            expect(TokenType::KEYWORD, "ROWS", "Expected ROWS after FETCH NEXT n");
            expect(TokenType::KEYWORD, "ONLY", "Expected ONLY after FETCH NEXT n ROWS");
        }
        return std::make_unique<LimitNode>(limit, offset);
    }
    return nullptr;
}

// === Expression parsing (Pratt/precedence climbing algorithm) ===

static const struct {
    std::string op;
    int precedence;
    bool rightAssoc;
} operatorPrecedence[] = {
    {"OR", 1, false},
    {"AND", 2, false},
    {"NOT", 3, true},
    {"=", 4, false}, {"<", 4, false}, {">", 4, false},
    {"<=", 4, false}, {">=", 4, false}, {"<>", 4, false}, {"!=", 4, false},
    {"IS", 4, false}, {"LIKE", 4, false}, {"IN", 4, false}, {"BETWEEN", 4, false},
    {"+", 5, false}, {"-", 5, false},
    {"*", 6, false}, {"/", 6, false}, {"%", 6, false},
    {"^", 7, true}
};

static int getPrecedence(const Token& token) {
    std::string val = token.getValue();
    for (const auto& op : operatorPrecedence) {
        if (val == op.op)
            return op.precedence;
    }
    return -1;
}
static bool isRightAssoc(const Token& token) {
    std::string val = token.getValue();
    for (const auto& op : operatorPrecedence) {
        if (val == op.op)
            return op.rightAssoc;
    }
    return false;
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(int precedence) {
    std::unique_ptr<ExpressionNode> left;

    // --- Primary expressions ---
    if (match(TokenType::PUNCTUATOR, "(")) {
        if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
            auto subquery = parseQuery();
            expect(TokenType::PUNCTUATOR, ")", "Expected ) after subquery");
            left = std::make_unique<SubqueryExprNode>(std::move(subquery));
        }
        else {
            left = std::make_unique<ParenthesizedExprNode>(parseExpression());
            expect(TokenType::PUNCTUATOR, ")", "Expected ) after expression");
        }
    }
    // EXISTS and NOT EXISTS support
    else if (currentToken().getType() == TokenType::KEYWORD &&
        (currentToken().getValue() == "EXISTS" || currentToken().getValue() == "NOT")) {
        bool isNot = false;
        if (currentToken().getValue() == "NOT") {
            advance();
            if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "EXISTS") {
                isNot = true;
            }
            else {
                error("Expected EXISTS after NOT");
            }
        }
        if (currentToken().getValue() == "EXISTS") {
            advance();
            expect(TokenType::PUNCTUATOR, "(", "Expected ( after EXISTS");
            if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
                auto subquery = parseQuery();
                expect(TokenType::PUNCTUATOR, ")", "Expected ) after EXISTS subquery");
                left = std::make_unique<ExistsExprNode>(std::move(subquery), isNot);
            }
            else {
                error("Expected SELECT after EXISTS (");
            }
        }
    }
    // ANY/ALL/SOME support as standalone (rare, but ANSI SQL allows it)
    else if (currentToken().getType() == TokenType::KEYWORD &&
        (currentToken().getValue() == "ANY" ||
            currentToken().getValue() == "ALL" ||
            currentToken().getValue() == "SOME")) {
        std::string quant = currentToken().getValue();
        advance();
        expect(TokenType::PUNCTUATOR, "(", "Expected ( after " + quant);
        if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
            auto subquery = parseQuery();
            expect(TokenType::PUNCTUATOR, ")", "Expected ) after " + quant + " subquery");
            left = std::make_unique<QuantifiedSubqueryNode>(quant, std::move(subquery));
        }
        else {
            error("Expected SELECT after " + quant + " (");
        }
    }
    // Literal, Function, Identifier, Star, Unary minus, CASE
    else if (currentToken().getType() == TokenType::LITERAL) {
        left = std::make_unique<LiteralNode>(currentToken().getValue(), "LITERAL");
        advance();
    }
    else if (currentToken().getType() == TokenType::FUNCTION) {
        std::string funcName = currentToken().getValue();
        advance();
        if (match(TokenType::PUNCTUATOR, "(")) {
            std::vector<std::unique_ptr<ExpressionNode>> args;
            if (!match(TokenType::PUNCTUATOR, ")")) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::PUNCTUATOR, ","));
                expect(TokenType::PUNCTUATOR, ")", "Expected ) after function arguments");
            }
            left = std::make_unique<FunctionCallNode>(funcName, std::move(args));
        }
        else {
            error("Expected '(' after function name");
        }
    }
    else if (currentToken().getType() == TokenType::IDENTIFIER) {
        std::string name = currentToken().getValue();
        advance();

        if (match(TokenType::PUNCTUATOR, ".")) {
            if (match(TokenType::OPERATOR, "*") || match(TokenType::PUNCTUATOR, "*")) {
                left = std::make_unique<StarNode>(name);
            }
            else if (currentToken().getType() == TokenType::IDENTIFIER) {
                std::string next = currentToken().getValue();
                advance();
                left = std::make_unique<IdentifierNode>(name + "." + next);
            }
            else {
                error("Expected * or identifier after .");
            }
        }
        else if (match(TokenType::PUNCTUATOR, "(")) {
            std::vector<std::unique_ptr<ExpressionNode>> args;
            if (!match(TokenType::PUNCTUATOR, ")")) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::PUNCTUATOR, ","));
                expect(TokenType::PUNCTUATOR, ")", "Expected ) after function arguments");
            }
            left = std::make_unique<FunctionCallNode>(name, std::move(args));
        }
        else {
            left = std::make_unique<IdentifierNode>(name);
        }
    }
    else if (match(TokenType::OPERATOR, "*") || match(TokenType::PUNCTUATOR, "*")) {
        left = std::make_unique<StarNode>();
    }
    else if (currentToken().getType() == TokenType::OPERATOR && currentToken().getValue() == "-") {
        advance();
        auto right = parseExpression(7);
        left = std::make_unique<OperatorNode>("-", std::make_unique<LiteralNode>("0", "LITERAL"), std::move(right));
    }
    else if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "CASE") {
        advance();
        auto caseNode = std::make_unique<CaseExpressionNode>();
        while (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "WHEN") {
            advance();
            auto whenExpr = parseExpression();
            expect(TokenType::KEYWORD, "THEN", "Expected THEN after WHEN expression");
            auto thenExpr = parseExpression();
            caseNode->cases.push_back({ std::move(whenExpr), std::move(thenExpr) });
        }
        if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "ELSE") {
            advance();
            caseNode->elseExpr = parseExpression();
        }
        expect(TokenType::KEYWORD, "END", "Expected END for CASE expression");
        left = std::move(caseNode);
    }
    else {
        error("Unexpected token in expression");
    }

    // --- Infix/postfix operators and predicates ---
    while (true) {
        TokenType ttype = currentToken().getType();
        std::string tval = currentToken().getValue();

        int tokenPrec = getPrecedence(currentToken());
        if (tokenPrec < precedence)
            break;

        if (ttype == TokenType::KEYWORD &&
            (tval == "BETWEEN" || tval == "IN" || tval == "IS" || tval == "LIKE" ||
                tval == "ANY" || tval == "ALL" || tval == "SOME")) {
            std::string op = tval;
            advance();
            if (op == "BETWEEN") {
                auto expr1 = parseExpression(tokenPrec + 1);
                expect(TokenType::KEYWORD, "AND", "Expected AND in BETWEEN");
                auto expr2 = parseExpression(tokenPrec + 1);
                left = std::make_unique<OperatorNode>("BETWEEN", std::move(left),
                    std::make_unique<OperatorNode>("AND", std::move(expr1), std::move(expr2)));
            }
            else if (op == "IN") {
                if (match(TokenType::PUNCTUATOR, "(")) {
                    std::vector<std::unique_ptr<ExpressionNode>> inList;
                    if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
                        // IN (SELECT ...)
                        auto subquery = parseQuery();
                        expect(TokenType::PUNCTUATOR, ")", "Expected ) after IN subquery");
                        inList.push_back(std::make_unique<SubqueryExprNode>(std::move(subquery)));
                    }
                    else {
                        do {
                            inList.push_back(parseExpression());
                        } while (match(TokenType::PUNCTUATOR, ","));
                        expect(TokenType::PUNCTUATOR, ")", "Expected ) after IN value list");
                    }
                    auto inOp = std::make_unique<FunctionCallNode>("IN", std::move(inList));
                    left = std::make_unique<OperatorNode>("IN", std::move(left), std::move(inOp));
                }
                else {
                    error("Expected ( after IN");
                }
            }
            else if (op == "IS") {
                bool notFlag = match(TokenType::KEYWORD, "NOT");
                if (match(TokenType::KEYWORD, "NULL")) {
                    auto right = std::make_unique<IdentifierNode>("NULL");
                    left = std::make_unique<OperatorNode>(notFlag ? "IS NOT" : "IS", std::move(left), std::move(right));
                }
                else {
                    error("Expected NULL after IS/IS NOT");
                }
            }
            else if (op == "LIKE") {
                auto pattern = parseExpression(tokenPrec + 1);
                left = std::make_unique<OperatorNode>("LIKE", std::move(left), std::move(pattern));
            }
            // Scalar comparison with quantified subquery: e.g. a > ANY (SELECT ...)
            else if (op == "ANY" || op == "ALL" || op == "SOME") {
                if (match(TokenType::PUNCTUATOR, "(")) {
                    if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
                        auto subquery = parseQuery();
                        expect(TokenType::PUNCTUATOR, ")", "Expected ) after " + op + " subquery");
                        left = std::make_unique<OperatorNode>(op, std::move(left),
                            std::make_unique<QuantifiedSubqueryNode>(op, std::move(subquery)));
                    }
                    else {
                        error("Expected SELECT after " + op + " (");
                    }
                }
                else {
                    error("Expected ( after " + op);
                }
            }
            continue;
        }

        if (ttype == TokenType::OPERATOR || (ttype == TokenType::KEYWORD && tokenPrec > 0)) {
            std::string op = tval;
            advance();
            int nextPrec = tokenPrec + (isRightAssoc(currentToken()) ? 0 : 1);
            auto right = parseExpression(nextPrec);
            left = std::make_unique<OperatorNode>(op, std::move(left), std::move(right));
            continue;
        }

        break;
    }

    return left;
}