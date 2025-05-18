// File: ASTNodes.hpp
// Purpose: Defines all AST node types for the SQL parser's Abstract Syntax Tree (AST).
// Each node type represents a component of SQL syntax, supporting traversal, printing, and the visitor pattern.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "ASTVisitor.hpp"  

// ===================== Base AST Node Classes =====================

/**
 * @brief Base class for all AST nodes.
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) { visitor.visit(*this); }
};

/**
 * @brief Base class for all expression nodes.
 */
class ExpressionNode : public ASTNode {};

// ===================== Expression Nodes =====================

/**
 * @brief Literal value node (e.g. '123', 'abc').
 */
class LiteralNode : public ExpressionNode {
public:
    std::string value;
    std::string type;
    LiteralNode(const std::string& value, const std::string& type)
        : value(value), type(type) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Identifier node (e.g. column, table name).
 */
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    IdentifierNode(const std::string& name) : name(name) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Represents '*' or 'table.*'.
 */
class StarNode : public ExpressionNode {
public:
    std::optional<std::string> tableName;
    StarNode() : tableName(std::nullopt) {}
    StarNode(const std::string& tbl) : tableName(tbl) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Parenthesized subexpression node.
 */
class ParenthesizedExprNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> expr;
    ParenthesizedExprNode(std::unique_ptr<ExpressionNode> expr)
        : expr(std::move(expr)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Subquery as an expression node.
 */
class SubqueryExprNode : public ExpressionNode {
public:
    std::unique_ptr<ASTNode> subquery;
    SubqueryExprNode(std::unique_ptr<ASTNode> subquery)
        : subquery(std::move(subquery)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief EXISTS/NOT EXISTS predicate node (EXISTS (subquery) or NOT EXISTS (subquery))
 */
class ExistsExprNode : public ExpressionNode {
public:
    std::unique_ptr<ASTNode> subquery;
    bool isNot;
    ExistsExprNode(std::unique_ptr<ASTNode> subquery, bool isNot = false)
        : subquery(std::move(subquery)), isNot(isNot) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief ANY/ALL/SOME quantified subquery node (e.g. > ANY (SELECT ...))
 */
class QuantifiedSubqueryNode : public ExpressionNode {
public:
    std::string quantifier; // "ANY", "ALL", "SOME"
    std::unique_ptr<ASTNode> subquery;
    QuantifiedSubqueryNode(const std::string& quant, std::unique_ptr<ASTNode> subquery)
        : quantifier(quant), subquery(std::move(subquery)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Function call node (e.g. SUM(col)).
 */
class FunctionCallNode : public ExpressionNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ExpressionNode>> args;
    FunctionCallNode(const std::string& functionName, std::vector<std::unique_ptr<ExpressionNode>> args)
        : functionName(functionName), args(std::move(args)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Operator node (unary or binary).
 */
class OperatorNode : public ExpressionNode {
public:
    std::string op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
    OperatorNode(const std::string& op,
        std::unique_ptr<ExpressionNode> left,
        std::unique_ptr<ExpressionNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief CASE WHEN ... THEN ... ELSE ... END expression node.
 */
class CaseExpressionNode : public ExpressionNode {
public:
    struct WhenThen {
        std::unique_ptr<ExpressionNode> when;
        std::unique_ptr<ExpressionNode> then;
    };
    std::vector<WhenThen> cases;
    std::unique_ptr<ExpressionNode> elseExpr;
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ===================== SELECT List, Table, Join =====================

/**
 * @brief Item in the SELECT list (expression [AS alias]).
 */
class SelectItemNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> expr;
    std::optional<std::string> alias;
    SelectItemNode(std::unique_ptr<ExpressionNode> expr, std::optional<std::string> alias)
        : expr(std::move(expr)), alias(std::move(alias)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Table reference or subquery in the FROM clause.
 */
class TableReferenceNode : public ASTNode {
public:
    std::string tableName;
    std::optional<std::string> alias;
    std::unique_ptr<ASTNode> subquery;
    TableReferenceNode(const std::string& name, std::optional<std::string> alias)
        : tableName(name), alias(std::move(alias)), subquery(nullptr) {
    }
    TableReferenceNode(std::unique_ptr<ASTNode> subquery, std::optional<std::string> alias)
        : tableName(), alias(std::move(alias)), subquery(std::move(subquery)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief JOIN clause node.
 */
class JoinNode : public ASTNode {
public:
    std::string joinType;
    std::unique_ptr<TableReferenceNode> left;
    std::unique_ptr<TableReferenceNode> right;
    std::unique_ptr<ExpressionNode> onExpr;
    JoinNode(const std::string& joinType,
        std::unique_ptr<TableReferenceNode> left,
        std::unique_ptr<TableReferenceNode> right,
        std::unique_ptr<ExpressionNode> onExpr)
        : joinType(joinType), left(std::move(left)), right(std::move(right)), onExpr(std::move(onExpr)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ===================== WHERE, GROUP BY, HAVING, ORDER BY, LIMIT =====================

/**
 * @brief WHERE clause node.
 */
class WhereNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    WhereNode(std::unique_ptr<ExpressionNode> cond) : condition(std::move(cond)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief GROUP BY clause node.
 */
class GroupByNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ExpressionNode>> groupExprs;
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief HAVING clause node.
 */
class HavingNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    HavingNode(std::unique_ptr<ExpressionNode> cond) : condition(std::move(cond)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief ORDER BY clause node.
 */
class OrderByNode : public ASTNode {
public:
    struct OrderItem {
        std::unique_ptr<ExpressionNode> expr;
        bool ascending = true;
    };
    std::vector<OrderItem> orderItems;
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief LIMIT/OFFSET clause node.
 */
class LimitNode : public ASTNode {
public:
    int limit;
    int offset;
    LimitNode(int limit, int offset) : limit(limit), offset(offset) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ===================== Set Operations and Query Root =====================

/**
 * @brief Types of set operations supported (UNION/INTERSECT/EXCEPT).
 */
enum class SetOperationType { UNION, INTERSECT, EXCEPT };

/**
 * @brief Set operation node (e.g., UNION, INTERSECT).
 */
class SetOperationNode : public ASTNode {
public:
    SetOperationType opType;
    bool all;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    SetOperationNode(SetOperationType opType, bool all,
        std::unique_ptr<ASTNode> left,
        std::unique_ptr<ASTNode> right)
        : opType(opType), all(all), left(std::move(left)), right(std::move(right)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief SELECT statement node (core SQL statement).
 */
class SelectStatementNode : public ASTNode {
public:
    bool distinct = false;
    std::optional<int> topN;
    std::vector<std::unique_ptr<SelectItemNode>> selectItems;
    std::vector<std::unique_ptr<TableReferenceNode>> from;
    std::vector<std::unique_ptr<JoinNode>> joins;
    std::unique_ptr<WhereNode> where;
    std::unique_ptr<GroupByNode> groupBy;
    std::unique_ptr<HavingNode> having;
    std::unique_ptr<OrderByNode> orderBy;
    std::unique_ptr<LimitNode> limit;

    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

/**
 * @brief Root node for a full query (single query or set operation).
 */
class QueryRootNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> child;
    QueryRootNode(std::unique_ptr<ASTNode> c) : child(std::move(c)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};