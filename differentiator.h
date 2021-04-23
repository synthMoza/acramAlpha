#pragma once

#include <cstring>

#include "token.h"
#include "vector.h"
#include "expression.h"

namespace se {
    const std::size_t max_str_size = 256;

    // Class that allows to differentiate the input expression
    class differentiator {
        // The original string
        char* str_;
        // The vector of tokens
        vector<token> tokens_;
        // The original expression
        expression expression_;
        // The differentiated expression
        expression diff_expression_;

        // Tokenize the expression
        void tokenize();
        // Methods for tokenizing
        // ==============================
        int get_token_op(); // operation token
        int get_token_num(); // number token
        int get_token_var(); // variable token
        // ==============================
        // Methods for differentiating
        // ==============================
        node* diff_exp(node* nd);
        // ==============================
    public:
        // Constructor of this class
        differentiator();
        // Set the string with the expression
        // Autimatically tokenizes it for further differentiation
        void set_expression(const char* input_str);
        // Differentiate the expression
        void diff();
        // Generate the output LaTeX file
        void generateLatex(const char* file_name);

        ~differentiator();
    };
};  