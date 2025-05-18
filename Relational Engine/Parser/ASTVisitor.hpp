#pragma once

class ASTNode;
class LiteralNode;
class IdentifierNode;
class StarNode;
class ParenthesizedExprNode;
class SubqueryExprNode;
class FunctionCallNode;
class OperatorNode;
class CaseExpressionNode;
class SelectItemNode;
class TableReferenceNode;
class JoinNode;
class WhereNode;
class GroupByNode;
class HavingNode;
class OrderByNode;
class LimitNode;
class SetOperationNode;
class SelectStatementNode;
class QueryRootNode;

// Visitor interface for the AST
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ASTNode&) {}
    virtual void visit(LiteralNode&) {}
    virtual void visit(IdentifierNode&) {}
    virtual void visit(StarNode&) {}
    virtual void visit(ParenthesizedExprNode&) {}
    virtual void visit(SubqueryExprNode&) {}
    virtual void visit(FunctionCallNode&) {}
    virtual void visit(OperatorNode&) {}
    virtual void visit(CaseExpressionNode&) {}
    virtual void visit(SelectItemNode&) {}
    virtual void visit(TableReferenceNode&) {}
    virtual void visit(JoinNode&) {}
    virtual void visit(WhereNode&) {}
    virtual void visit(GroupByNode&) {}
    virtual void visit(HavingNode&) {}
    virtual void visit(OrderByNode&) {}
    virtual void visit(LimitNode&) {}
    virtual void visit(SetOperationNode&) {}
    virtual void visit(SelectStatementNode&) {}
    virtual void visit(QueryRootNode&) {}
};