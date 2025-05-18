// File: AST.hpp
// Purpose: Defines the AST class, which owns the root node and provides utility functions for the full AST tree.

#pragma once

#include "ASTNodes.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <functional>

// Forward declare ASTVisitor to avoid circular include
class ASTVisitor;

/**
 * @brief The AST class holds the root of the SQL AST and provides utility functions.
 */
class AST {
public:
    /**
     * @brief Constructor (takes ownership).
     * @param root The root node of the AST.
     */
    explicit AST(std::unique_ptr<ASTNode> root);

    /**
     * @brief Access root node (read-only and mutable overloads).
     */
    const ASTNode* getRoot() const;
    ASTNode* getRoot();

    /**
     * @brief Replace the root node (ownership transfer).
     */
    void setRoot(std::unique_ptr<ASTNode> newRoot);

    /**
     * @brief Print or serialize AST.
     */
    std::string toString() const;

    /**
     * @brief Accept a visitor (visitor pattern).
     */
    void accept(ASTVisitor& visitor);

    /**
     * @brief Traverse the AST (pre-order), calling a function for each node.
     */
    void traverse(const std::function<void(ASTNode&)>& fn) const;

    /**
     * @brief Print the AST tree to a stream with indentation.
     * @param out Output stream (e.g., std::cout)
     */
    void print(std::ostream& out = std::cout) const;

private:
    std::unique_ptr<ASTNode> root;
    void traverseNode(ASTNode* node, const std::function<void(ASTNode&)>& fn) const;

    // Print helper
    void printNode(const ASTNode* node, std::ostream& out, int indent = 0) const;
};