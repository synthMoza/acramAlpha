#pragma once

#include <cstring>

#include "token.h"
#include "vector.h"
#include "expression.h"

namespace se {
    // Class that allows to differentiate the input expression
    class differentiator {
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
        node* diff_num(node* nd);
        node* diff_var(node* nd);
        node* diff_plus_minus(node* nd);
        node* diff_mul(node* nd);
        node* diff_pow(node* nd);
        node* diff_sin(node* nd);
        node* diff_cos(node* nd);
        node* diff_div(node* nd);
        node* diff_ln(node* nd);
        node* diff_tg(node* nd);
        node* diff_ctg(node* nd);
        // ==============================
    public:
        // Set the string with the expression
        void set_expression(const char* input_str);
        // Differentiate the expression
        void diff();
        // Generate the output LaTeX file
        void generateLatex(const char* file_name);
    };
};  