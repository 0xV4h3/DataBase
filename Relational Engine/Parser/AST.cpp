// File: AST.cpp
// Purpose: Implements the AST class for SQL parsing.

#include "AST.hpp"

AST::AST(std::unique_ptr<ASTNode> root)
    : root(std::move(root)) {
}

const ASTNode* AST::getRoot() const {
    return root.get();
}
ASTNode* AST::getRoot() {
    return root.get();
}

void AST::setRoot(std::unique_ptr<ASTNode> newRoot) {
    root = std::move(newRoot);
}

std::string AST::toString() const {
    return root ? root->toString() : "<empty AST>";
}

void AST::accept(ASTVisitor& visitor) {
    if (root) {
        root->accept(visitor);
    }
}

void AST::traverse(const std::function<void(ASTNode&)>& fn) const {
    traverseNode(root.get(), fn);
}

void AST::traverseNode(ASTNode* node, const std::function<void(ASTNode&)>& fn) const {
    if (!node) return;
    fn(*node);
    // Recursively traverse children by dynamic_cast and downcasting for known node types.
    // Optionally expand this for more node types as your AST grows.
    if (auto select = dynamic_cast<SelectStatementNode*>(node)) {
        for (auto& item : select->selectItems)
            traverseNode(item.get(), fn);
        for (auto& table : select->from)
            traverseNode(table.get(), fn);
        for (auto& join : select->joins)
            traverseNode(join.get(), fn);
        if (select->where)
            traverseNode(select->where.get(), fn);
        if (select->groupBy)
            traverseNode(select->groupBy.get(), fn);
        if (select->having)
            traverseNode(select->having.get(), fn);
        if (select->orderBy)
            traverseNode(select->orderBy.get(), fn);
        if (select->limit)
            traverseNode(select->limit.get(), fn);
    }
    // Repeat for other composite node types (e.g., SetOperationNode, QueryRootNode, etc.)
    if (auto setop = dynamic_cast<SetOperationNode*>(node)) {
        traverseNode(setop->left.get(), fn);
        traverseNode(setop->right.get(), fn);
    }
    if (auto queryRoot = dynamic_cast<QueryRootNode*>(node)) {
        traverseNode(queryRoot->child.get(), fn);
    }
    if (auto selectItem = dynamic_cast<SelectItemNode*>(node)) {
        traverseNode(selectItem->expr.get(), fn);
    }
    if (auto tableRef = dynamic_cast<TableReferenceNode*>(node)) {
        if (tableRef->subquery)
            traverseNode(tableRef->subquery.get(), fn);
    }
    if (auto join = dynamic_cast<JoinNode*>(node)) {
        traverseNode(join->left.get(), fn);
        traverseNode(join->right.get(), fn);
        traverseNode(join->onExpr.get(), fn);
    }
    if (auto where = dynamic_cast<WhereNode*>(node)) {
        traverseNode(where->condition.get(), fn);
    }
    if (auto group = dynamic_cast<GroupByNode*>(node)) {
        for (auto& e : group->groupExprs)
            traverseNode(e.get(), fn);
    }
    if (auto having = dynamic_cast<HavingNode*>(node)) {
        traverseNode(having->condition.get(), fn);
    }
    if (auto order = dynamic_cast<OrderByNode*>(node)) {
        for (auto& item : order->orderItems)
            traverseNode(item.expr.get(), fn);
    }
    if (auto limit = dynamic_cast<LimitNode*>(node)) {
        // No children
    }
    // Expressions (expand as you add node types)
    if (auto op = dynamic_cast<OperatorNode*>(node)) {
        traverseNode(op->left.get(), fn);
        traverseNode(op->right.get(), fn);
    }
    if (auto func = dynamic_cast<FunctionCallNode*>(node)) {
        for (auto& arg : func->args)
            traverseNode(arg.get(), fn);
    }
    if (auto paren = dynamic_cast<ParenthesizedExprNode*>(node)) {
        traverseNode(paren->expr.get(), fn);
    }
    if (auto caseExpr = dynamic_cast<CaseExpressionNode*>(node)) {
        for (const auto& c : caseExpr->cases) {
            traverseNode(c.when.get(), fn);
            traverseNode(c.then.get(), fn);
        }
        if (caseExpr->elseExpr)
            traverseNode(caseExpr->elseExpr.get(), fn);
    }
    if (auto subq = dynamic_cast<SubqueryExprNode*>(node)) {
        traverseNode(subq->subquery.get(), fn);
    }
    // === New nodes: ExistsExprNode and QuantifiedSubqueryNode
    if (auto existsNode = dynamic_cast<ExistsExprNode*>(node)) {
        traverseNode(existsNode->subquery.get(), fn);
    }
    if (auto quantNode = dynamic_cast<QuantifiedSubqueryNode*>(node)) {
        traverseNode(quantNode->subquery.get(), fn);
    }
    // Leaf nodes: IdentifierNode, LiteralNode, StarNode, etc. -- nothing to do.
}

void AST::print(std::ostream& out) const {
    printNode(root.get(), out, 0);
}

void AST::printNode(const ASTNode* node, std::ostream& out, int indent) const {
    if (!node) return;
    std::string ind(indent, ' ');

    // Print node type and key info
    out << ind << node->toString() << "  [" << typeid(*node).name() << "]\n";

    // Recursively print children based on node type
    if (auto select = dynamic_cast<const SelectStatementNode*>(node)) {
        for (const auto& item : select->selectItems) {
            printNode(item.get(), out, indent + 2);
        }
        for (const auto& table : select->from) {
            printNode(table.get(), out, indent + 2);
        }
        for (const auto& join : select->joins) {
            printNode(join.get(), out, indent + 2);
        }
        printNode(select->where.get(), out, indent + 2);
        printNode(select->groupBy.get(), out, indent + 2);
        printNode(select->having.get(), out, indent + 2);
        printNode(select->orderBy.get(), out, indent + 2);
        printNode(select->limit.get(), out, indent + 2);
    }
    else if (auto setop = dynamic_cast<const SetOperationNode*>(node)) {
        printNode(setop->left.get(), out, indent + 2);
        printNode(setop->right.get(), out, indent + 2);
    }
    else if (auto queryRoot = dynamic_cast<const QueryRootNode*>(node)) {
        printNode(queryRoot->child.get(), out, indent + 2);
    }
    else if (auto selectItem = dynamic_cast<const SelectItemNode*>(node)) {
        printNode(selectItem->expr.get(), out, indent + 2);
    }
    else if (auto tableRef = dynamic_cast<const TableReferenceNode*>(node)) {
        printNode(tableRef->subquery.get(), out, indent + 2);
    }
    else if (auto join = dynamic_cast<const JoinNode*>(node)) {
        printNode(join->left.get(), out, indent + 2);
        printNode(join->right.get(), out, indent + 2);
        printNode(join->onExpr.get(), out, indent + 2);
    }
    else if (auto where = dynamic_cast<const WhereNode*>(node)) {
        printNode(where->condition.get(), out, indent + 2);
    }
    else if (auto group = dynamic_cast<const GroupByNode*>(node)) {
        for (const auto& e : group->groupExprs)
            printNode(e.get(), out, indent + 2);
    }
    else if (auto having = dynamic_cast<const HavingNode*>(node)) {
        printNode(having->condition.get(), out, indent + 2);
    }
    else if (auto order = dynamic_cast<const OrderByNode*>(node)) {
        for (const auto& item : order->orderItems)
            printNode(item.expr.get(), out, indent + 2);
    }
    else if (auto op = dynamic_cast<const OperatorNode*>(node)) {
        printNode(op->left.get(), out, indent + 2);
        printNode(op->right.get(), out, indent + 2);
    }
    else if (auto func = dynamic_cast<const FunctionCallNode*>(node)) {
        for (const auto& arg : func->args)
            printNode(arg.get(), out, indent + 2);
    }
    else if (auto paren = dynamic_cast<const ParenthesizedExprNode*>(node)) {
        printNode(paren->expr.get(), out, indent + 2);
    }
    else if (auto caseExpr = dynamic_cast<const CaseExpressionNode*>(node)) {
        for (const auto& c : caseExpr->cases) {
            printNode(c.when.get(), out, indent + 2);
            printNode(c.then.get(), out, indent + 2);
        }
        printNode(caseExpr->elseExpr.get(), out, indent + 2);
    }
    else if (auto subq = dynamic_cast<const SubqueryExprNode*>(node)) {
        printNode(subq->subquery.get(), out, indent + 2);
    }
    // === New nodes: ExistsExprNode and QuantifiedSubqueryNode
    else if (auto existsNode = dynamic_cast<const ExistsExprNode*>(node)) {
        printNode(existsNode->subquery.get(), out, indent + 2);
    }
    else if (auto quantNode = dynamic_cast<const QuantifiedSubqueryNode*>(node)) {
        printNode(quantNode->subquery.get(), out, indent + 2);
    }
    // Leaf nodes: IdentifierNode, LiteralNode, StarNode, etc. -- nothing more to print
}