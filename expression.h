#pragma once

#include "token.h"
#include "vector.h"

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

        // Static method to get empty node
        static node* get_node();

        ~node();
    };

    // Class that enables to build expression trees out of token vectors
    // and work with them
    class expression {
        node* tree_; // the expression tree
        std::size_t exp_counter_; // iterating index for several methods

        // Recursive descent methods
        // ==================================
        node* getPlusMinus(const vector<token>& tokens);
        node* getNum(const vector<token>& tokens);
        // ==================================
        // Recursive method for calculating expression
        double calculate(node* nd, char var = '\0', double value = 0);
    public:
        expression();

        // Build the expression tree of this vector of tokens
        void build_tree(const vector<token>& tokens);
        // Return the current expression tree
        node* get_tree();
        // Return the value of this expression
        // If it can not be calculated (includes a variable), set variable's value to the given argument
        double calculate(char var = '\0', double value = 0);
        ~expression();
    };
};