#pragma once

#include "token.h"
#include "vector.h"

// Variable to differentiate by
#define VAR "x"

namespace se {
    // Structure that represents the node of the expression tree
    struct node {
        token tk; // the value of the node is the token
        // Children
        node* leftChild_;
        node* rightChild_;

        node();
        // Set the value of this node to this token
        void set_token(const token& rhs);
        // Copy this tree
        node* copy();

        // Static method to get empty node
        static node* get_node();

        ~node();
    private:
        node* copy(node* nd);
    };

    // Class that enables to build expression trees out of token vectors
    // and work with them
    class expression {
        node* tree_; // the expression tree
        std::size_t exp_counter_; // iterating index for several methods

        // Recursive descent methods
        // ==================================
        node* getPlusMinus(const vector<token>& tokens);
        node* getMulDiv(const vector<token>& tokens);
        node* getBrackets(const vector<token>& tokens);
        node* getNumVar(const vector<token>& tokens);
        // ==================================
        // Recursive method for calculating expression
        double calculate(node* nd, double value = 0);
        // LaTeX output recursive method
        void latex_print(FILE* file, node* nd);
    public:
        expression();

        // Build the expression tree of this vector of tokens
        void build_tree(const vector<token>& tokens);
        // Return the current expression tree
        node* get_tree();
        // Set the new expression tree
        void set_tree(node* nd);
        // Return the value of this expression
        // If it can not be calculated (includes a variable), set variable's value to the given argument
        double calculate(double value = 0);
        // Output this expression into the LaTeX file
        void latexOutput(const char* file_name);
        ~expression();
    };
};