#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "ASTVisitor.hpp"  

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) { visitor.visit(*this); }
};

class ExpressionNode : public ASTNode {};

// Literal (e.g. '123', 'abc')
class LiteralNode : public ExpressionNode {
public:
    std::string value;
    std::string type; // e.g. "INTEGER", "STRING", etc.
    LiteralNode(const std::string& value, const std::string& type)
        : value(value), type(type) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Identifier (e.g. column, table name)
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    IdentifierNode(const std::string& name) : name(name) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// SELECT * or Table.*
class StarNode : public ExpressionNode {
public:
    std::optional<std::string> tableName; // nullopt for *, tableName for Table.*
    StarNode() : tableName(std::nullopt) {}
    StarNode(const std::string& tbl) : tableName(tbl) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Parenthesized expression (preserves grouping/precedence)
class ParenthesizedExprNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> expr;
    ParenthesizedExprNode(std::unique_ptr<ExpressionNode> expr)
        : expr(std::move(expr)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// (Subquery as an expression, e.g. in SELECT or WHERE)
class SubqueryExprNode : public ExpressionNode {
public:
    std::unique_ptr<ASTNode> subquery;
    SubqueryExprNode(std::unique_ptr<ASTNode> subquery)
        : subquery(std::move(subquery)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Function call e.g. SUM(col)
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

// Operator (binary or unary)
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

// CASE WHEN ... THEN ... ELSE ... END
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

// ========== SELECT List Item ==========
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

// ========== Table Reference & Join ==========

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

class JoinNode : public ASTNode {
public:
    std::string joinType; // e.g. "INNER", "LEFT OUTER"
    std::unique_ptr<TableReferenceNode> left;
    std::unique_ptr<TableReferenceNode> right;
    std::unique_ptr<ExpressionNode> onExpr; // ON condition
    JoinNode(const std::string& joinType,
        std::unique_ptr<TableReferenceNode> left,
        std::unique_ptr<TableReferenceNode> right,
        std::unique_ptr<ExpressionNode> onExpr)
        : joinType(joinType), left(std::move(left)), right(std::move(right)), onExpr(std::move(onExpr)) {
    }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ========== WHERE, GROUP BY, HAVING, ORDER BY, LIMIT ==========

class WhereNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    WhereNode(std::unique_ptr<ExpressionNode> cond) : condition(std::move(cond)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class GroupByNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ExpressionNode>> groupExprs;
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class HavingNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    HavingNode(std::unique_ptr<ExpressionNode> cond) : condition(std::move(cond)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

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

class LimitNode : public ASTNode {
public:
    int limit;
    int offset;
    LimitNode(int limit, int offset) : limit(limit), offset(offset) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ========== Set Operations (UNION, INTERSECT, EXCEPT) ==========

enum class SetOperationType { UNION, INTERSECT, EXCEPT };

class SetOperationNode : public ASTNode {
public:
    SetOperationType opType;
    bool all; // e.g. UNION ALL
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

// ========== Main Query Node ==========

class SelectStatementNode : public ASTNode {
public:
    bool distinct = false;
    std::optional<int> topN; // For TOP N queries (SQL Server style)
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

// ========== Root Node for Full Query (could be a single query or set operation) ==========

class QueryRootNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> child;
    QueryRootNode(std::unique_ptr<ASTNode> c) : child(std::move(c)) {}
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};