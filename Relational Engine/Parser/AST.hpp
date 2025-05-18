#pragma once

#include "ASTNodes.hpp"
#include <memory>
#include <string>
#include <functional>

/**
 * The AST class holds the root of the SQL AST and provides utility functions.
 */
class AST {
public:
    // Constructor (takes ownership)
    explicit AST(std::unique_ptr<ASTNode> root);

    // Access root node (read-only)
    const ASTNode* getRoot() const;
    ASTNode* getRoot();

    // Replace the root node (ownership transfer)
    void setRoot(std::unique_ptr<ASTNode> newRoot);

    // Print or serialize AST
    std::string toString() const;

    // Accept a visitor (visitor pattern)
    void accept(ASTVisitor& visitor);

    // Traverse the AST (pre-order), calling a function for each node
    void traverse(const std::function<void(ASTNode&)>& fn);

private:
    std::unique_ptr<ASTNode> root;
    void traverseNode(ASTNode* node, const std::function<void(ASTNode&)>& fn);
};