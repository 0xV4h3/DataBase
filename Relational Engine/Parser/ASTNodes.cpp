#include "ASTNodes.hpp"
#include <sstream>

// ===================== Literals, Identifiers, Stars ======================
std::string LiteralNode::toString() const {
    return "'" + value + "'";
}
std::string IdentifierNode::toString() const {
    return name;
}
std::string StarNode::toString() const {
    return tableName ? *tableName + ".*" : "*";
}

// ===================== Parenthesized, SubqueryExpr ======================
std::string ParenthesizedExprNode::toString() const {
    return "(" + (expr ? expr->toString() : "") + ")";
}
std::string SubqueryExprNode::toString() const {
    return "(" + (subquery ? subquery->toString() : "") + ")";
}

// ===================== Function Call ======================
std::string FunctionCallNode::toString() const {
    std::ostringstream oss;
    oss << functionName << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << (args[i] ? args[i]->toString() : "");
    }
    oss << ")";
    return oss.str();
}

// ===================== Operator ======================
std::string OperatorNode::toString() const {
    if (!left && !right) return op;
    if (!right) return op + (left ? " " + left->toString() : "");
    if (!left)  return op + " " + right->toString();
    return "(" + left->toString() + " " + op + " " + right->toString() + ")";
}

// ===================== CASE Expression ======================
std::string CaseExpressionNode::toString() const {
    std::ostringstream oss;
    oss << "CASE ";
    for (const auto& c : cases) {
        oss << "WHEN " << (c.when ? c.when->toString() : "")
            << " THEN " << (c.then ? c.then->toString() : "") << " ";
    }
    if (elseExpr) {
        oss << "ELSE " << elseExpr->toString() << " ";
    }
    oss << "END";
    return oss.str();
}

// ===================== Select Item ======================
std::string SelectItemNode::toString() const {
    std::ostringstream oss;
    oss << (expr ? expr->toString() : "");
    if (alias) oss << " AS " << *alias;
    return oss.str();
}

// ===================== Table Reference ======================
std::string TableReferenceNode::toString() const {
    if (subquery) {
        std::ostringstream oss;
        oss << "(" << subquery->toString() << ")";
        if (alias) oss << " AS " << *alias;
        return oss.str();
    }
    std::ostringstream oss;
    oss << tableName;
    if (alias) oss << " AS " << *alias;
    return oss.str();
}

// ===================== Join ======================
std::string JoinNode::toString() const {
    std::ostringstream oss;
    oss << (left ? left->toString() : "") << " " << joinType << " JOIN "
        << (right ? right->toString() : "");
    if (onExpr) oss << " ON " << onExpr->toString();
    return oss.str();
}

// ===================== Where, GroupBy, Having, OrderBy, Limit ======================
std::string WhereNode::toString() const {
    return "WHERE " + (condition ? condition->toString() : "");
}
std::string GroupByNode::toString() const {
    std::ostringstream oss;
    oss << "GROUP BY ";
    for (size_t i = 0; i < groupExprs.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << (groupExprs[i] ? groupExprs[i]->toString() : "");
    }
    return oss.str();
}
std::string HavingNode::toString() const {
    return "HAVING " + (condition ? condition->toString() : "");
}
std::string OrderByNode::toString() const {
    std::ostringstream oss;
    oss << "ORDER BY ";
    for (size_t i = 0; i < orderItems.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << (orderItems[i].expr ? orderItems[i].expr->toString() : "");
        oss << (orderItems[i].ascending ? " ASC" : " DESC");
    }
    return oss.str();
}
std::string LimitNode::toString() const {
    std::ostringstream oss;
    oss << "LIMIT " << limit;
    if (offset > 0) oss << " OFFSET " << offset;
    return oss.str();
}

// ===================== Set Operation ======================
std::string SetOperationNode::toString() const {
    std::ostringstream oss;
    if (left) oss << left->toString() << " ";
    switch (opType) {
    case SetOperationType::UNION: oss << "UNION"; break;
    case SetOperationType::INTERSECT: oss << "INTERSECT"; break;
    case SetOperationType::EXCEPT: oss << "EXCEPT"; break;
    }
    if (all) oss << " ALL";
    if (right) oss << " " << right->toString();
    return oss.str();
}

// ===================== Select Statement ======================
std::string SelectStatementNode::toString() const {
    std::ostringstream oss;
    oss << "SELECT ";
    if (distinct) oss << "DISTINCT ";
    if (topN) oss << "TOP " << *topN << " ";
    for (size_t i = 0; i < selectItems.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << (selectItems[i] ? selectItems[i]->toString() : "");
    }
    if (!from.empty()) {
        oss << " FROM ";
        for (size_t i = 0; i < from.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << (from[i] ? from[i]->toString() : "");
        }
    }
    for (const auto& join : joins) {
        if (join) oss << " " << join->toString();
    }
    if (where) oss << " " << where->toString();
    if (groupBy) oss << " " << groupBy->toString();
    if (having) oss << " " << having->toString();
    if (orderBy) oss << " " << orderBy->toString();
    if (limit) oss << " " << limit->toString();
    return oss.str();
}

// ===================== Query Root ======================
std::string QueryRootNode::toString() const {
    return child ? child->toString() : "";
}