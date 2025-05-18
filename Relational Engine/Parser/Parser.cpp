#include "Parser.hpp"
#include <sstream>
#include <cctype>

// --- Constructor ---
Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {
}

// --- Navigation helpers ---

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

// --- Error handling ---
[[noreturn]] void Parser::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "Parse error at token "
        << pos << " [value: '" << (isAtEnd() ? "<EOF>" : currentToken().getValue())
        << "']: " << message;
    throw std::runtime_error(oss.str());
}

// --- Multi-token keyword helpers ---
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

// --- Top-level parsing ---
std::unique_ptr<ASTNode> Parser::parse() {
    return parseQuery();
}

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

std::unique_ptr<SelectStatementNode> Parser::parseSelectStatement() {
    auto node = std::make_unique<SelectStatementNode>();

    // DISTINCT
    if (match(TokenType::KEYWORD, "DISTINCT"))
        node->distinct = true;

    // TOP N (if present)
    if (match(TokenType::KEYWORD, "TOP")) {
        if (currentToken().getType() == TokenType::LITERAL) {
            node->topN = std::stoi(currentToken().getValue());
            advance();
        }
        else {
            error("Expected number after TOP.");
        }
    }

    // SELECT list
    node->selectItems = parseSelectList();

    // FROM clause
    if (match(TokenType::KEYWORD, "FROM")) {
        node->from = parseFromClause();
        // JOINs (attached to FROM tables)
        node->joins = parseJoinClauses(node->from);
    }

    // WHERE
    node->where = parseWhereClause();

    // GROUP BY
    node->groupBy = parseGroupByClause();

    // HAVING
    node->having = parseHavingClause();

    // ORDER BY
    node->orderBy = parseOrderByClause();

    // LIMIT/OFFSET
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
    // Handles: expr [AS alias]
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
        // Alias without AS (optional)
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
    // Table name or subquery
    if (match(TokenType::PUNCTUATOR, "(")) {
        // Subquery as table source
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
        // Support multi-token join types (LEFT OUTER JOIN, RIGHT JOIN, etc.)
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

        // Next: right side table
        auto right = parseTableReference();

        // ON condition
        expect(TokenType::KEYWORD, "ON", "Expected ON after JOIN");
        auto onExpr = parseExpression();

        // Left side is last table in tables vector
        if (tables.empty())
            error("No table to join from (JOIN clause)");
        auto left = std::move(tables.back());
        tables.pop_back();
        joins.push_back(std::make_unique<JoinNode>(joinType, std::move(left), std::move(right), std::move(onExpr)));
        // For chaining, treat the join as the new left table for subsequent joins
        tables.push_back(std::make_unique<TableReferenceNode>(joins.back()->toString(), std::nullopt)); // Dummy node for chaining
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
                item.ascending = true; // Default
            }
            node->orderItems.push_back(std::move(item));
        } while (match(TokenType::PUNCTUATOR, ","));
        return node;
    }
    return nullptr;
}

std::unique_ptr<LimitNode> Parser::parseLimitClause() {
    // LIMIT [offset,] row_count  | OFFSET n ROWS FETCH NEXT n ROWS ONLY
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
            // LIMIT offset, row_count
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

// Operator precedence table for SQL (lowest to highest)
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

// Forward declaration for recursive parsing
std::unique_ptr<ExpressionNode> Parser::parseExpression(int precedence) {
    // Parse a primary expression first
    std::unique_ptr<ExpressionNode> left;

    // --- Primary Expressions ---
    if (match(TokenType::PUNCTUATOR, "(")) {
        // Parenthesized expression or subquery
        if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
            // Subquery as expression
            auto subquery = parseQuery();
            expect(TokenType::PUNCTUATOR, ")", "Expected ) after subquery");
            left = std::make_unique<SubqueryExprNode>(std::move(subquery));
        }
        else {
            // Parenthesized normal expression
            left = std::make_unique<ParenthesizedExprNode>(parseExpression());
            expect(TokenType::PUNCTUATOR, ")", "Expected ) after expression");
        }
    }
    else if (currentToken().getType() == TokenType::LITERAL) {
        left = std::make_unique<LiteralNode>(currentToken().getValue(), "LITERAL"); // You can pass actual type if available
        advance();
    }
    else if (currentToken().getType() == TokenType::IDENTIFIER) {
        // Identifier, function call, or Table.*
        std::string name = currentToken().getValue();
        advance();

        if (match(TokenType::PUNCTUATOR, ".")) {
            if (match(TokenType::OPERATOR, "*") || match(TokenType::PUNCTUATOR, "*")) {
                // Table.*
                left = std::make_unique<StarNode>(name);
            }
            else if (currentToken().getType() == TokenType::IDENTIFIER) {
                // Schema.Table or Table.Column (treat as identifier chain)
                std::string next = currentToken().getValue();
                advance();
                left = std::make_unique<IdentifierNode>(name + "." + next);
            }
            else {
                error("Expected * or identifier after .");
            }
        }
        else if (match(TokenType::PUNCTUATOR, "(")) {
            // Function call
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
            // Simple identifier
            left = std::make_unique<IdentifierNode>(name);
        }
    }
    else if (match(TokenType::OPERATOR, "*") || match(TokenType::PUNCTUATOR, "*")) {
        left = std::make_unique<StarNode>();
    }
    else if (currentToken().getType() == TokenType::OPERATOR && currentToken().getValue() == "-") {
        // Unary minus
        advance();
        auto right = parseExpression(7); // precedence of unary minus is high
        left = std::make_unique<OperatorNode>("-", std::make_unique<LiteralNode>("0", "LITERAL"), std::move(right));
    }
    else if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "CASE") {
        // CASE WHEN ... THEN ... [ELSE ...] END
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

    // --- Infix Operator Parsing (Pratt/precedence climbing) ---
    while (true) {
        // Check if current token is an operator or a special keyword operator
        TokenType ttype = currentToken().getType();
        std::string tval = currentToken().getValue();

        int tokenPrec = getPrecedence(currentToken());
        if (tokenPrec < precedence)
            break;

        // Special: handle BETWEEN, IN, IS, LIKE as infix
        if (ttype == TokenType::KEYWORD &&
            (tval == "BETWEEN" || tval == "IN" || tval == "IS" || tval == "LIKE")) {
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
                    // Could be value list or subquery
                    std::vector<std::unique_ptr<ExpressionNode>> inList;
                    if (currentToken().getType() == TokenType::KEYWORD && currentToken().getValue() == "SELECT") {
                        // Subquery
                        auto subquery = parseQuery();
                        expect(TokenType::PUNCTUATOR, ")", "Expected ) after IN subquery");
                        inList.push_back(std::make_unique<SubqueryExprNode>(std::move(subquery)));
                    }
                    else {
                        // List
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
                // IS [NOT] NULL
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
            continue;
        }

        // Standard infix operator
        if (ttype == TokenType::OPERATOR || (ttype == TokenType::KEYWORD && tokenPrec > 0)) {
            std::string op = tval;
            advance();
            int nextPrec = tokenPrec + (isRightAssoc(currentToken()) ? 0 : 1);
            auto right = parseExpression(nextPrec);
            left = std::make_unique<OperatorNode>(op, std::move(left), std::move(right));
            continue;
        }

        // No more binary/infix operator at this precedence
        break;
    }

    return left;
}