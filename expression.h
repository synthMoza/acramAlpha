#pragma once

#include <cmath>
#include <cstring>

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
        node(const node& rhs);
        node(node&& rhs);
        node& operator=(const node& rhs);
        node& operator=(node&& rhs);

        // Swap all members of these objects
        void swap(node& rhs);
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

    // Maximum input expression size
    const std::size_t max_str_size = 256;

    // Class that enables to build expression trees out of token vectors
    // and work with them
    class expression {
        char* str_; // the expression string
        node* tree_; // the expression tree
        std::size_t exp_counter_; // iterating index for several methods
        vector<token> tokens_; // vector of tokens from the input string

        // Tokenization methods
        // ==================================
        void tokenize();
        int get_token_op(); // operation token
        int get_token_num(); // number token
        int get_token_var(); // variable token
        // ==================================
        // Recursive descent methods
        // ==================================
        node* getPlusMinus();
        node* getMulDiv();
        node* getPow();
        node* getMathFunc();
        node* getBrackets();
        node* getNumVar();
        // ==================================
        // Recursive method for calculating expression
        double calculate(node* nd);
        // LaTeX output recursive methods
        // ==================================
        void latex_print(FILE* file, node* nd);
        void latex_print_plus(FILE* file, node* nd);
        void latex_print_minus(FILE* file, node* nd);
        void latex_print_mul(FILE* file, node* nd);
        void latex_print_div(FILE* file, node* nd);
        void latex_print_pow(FILE* file, node* nd);
        void latex_print_mathfunc(FILE* file, node* nd, const char* func);
        // ==================================
        // Simplify the expression
        node* simplify(node* nd);

        bool simpled_;
    public:
        expression();
        expression(const expression& rhs);
        expression(expression&& rhs);
        expression& operator=(const expression& rhs);
        expression& operator=(expression&& rhs);

        // Swap all members of these objects
        void swap(expression& rhs) noexcept;
        // Set the expression string
        void set_string(const char* input_str);
        // Simplify the expression
        void simplify();
        // Build the expression tree of this vector of tokens
        void build_tree(const char* input_str);
        // Return the current expression tree
        node* get_tree();
        // Set the new expression tree
        void set_tree(node* nd);
        // Return the value of this expression
        // If it can not be calculated (includes a variable), set variable's value to the given argument
        double calculate();
        // Output this expression into the LaTeX file
        void latexOutput(FILE* file);
        ~expression();
    };
};