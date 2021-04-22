#pragma once

#include <cstring>

#include "token.h"
#include "vector.h"
#include "expression.h"

namespace se {
    const std::size_t max_str_size = 256;

    // Class that allows to differentiate the input expression
    class differentiator {
        // The input string
        char* str_;
        // The vector of tokens
        vector<token> tokens_;
        // Expression
        expression expression_;
        // Variable to differentiate the expression by
        char var_;

        // Tokenize the expression
        void tokenize();
        // Methods for tokenizing
        // ==============================
        int get_token_op(); // operation token
        int get_token_num(); // number token
        // ==============================
    public:
        // Constructor of this class
        differentiator();
        // Set the string with the expression
        // Autimatically tokenizes it for further differentiation
        void set_expression(const char* input_str);
        // Set the variable for differentiating
        void set_variable(char var);
        // Differentiate the expression
        void diff();

        ~differentiator();
    };
};  