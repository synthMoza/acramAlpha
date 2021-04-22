#include "differentiator.h"

using namespace se;

differentiator::differentiator() : str_ (new char[max_str_size]), expression_(), var_ ('x') {}

void differentiator::set_expression(const char* input_str) {
    strcpy(str_, input_str);
    tokenize(); // first tokenize the expression
    expression_.build_tree(tokens_); // built the expression tree
    std::cout << "Value: " << expression_.calculate() << std::endl;
}

void differentiator::set_variable(char var) {
    var_ = var;
}

int differentiator::get_token_op() {
    int consumed = 0;
    token tk;

    #define DEF_OP(name, value)                                 \
        sscanf(str_, value "%n", &consumed);                    \
        if (consumed > 0) {                                     \
            tk.type = token_type::TOKEN_OP;                     \
            tk.op_id = token_op_id::OP_##name;                  \
            goto token_consumed;                                \
        }
    #include "def_op.h"
    #undef DEF_OP
    // Failed to consume this token
    return 1;
token_consumed:
    tokens_.push_back(tk);
    str_ += consumed;
    return 0;
}

int differentiator::get_token_num() {
    int consumed = 0;
    double value = 0;
    token tk;

    sscanf(str_, "%lg%n", &value, &consumed);
    if (consumed > 0) {
        tk.type = token_type::TOKEN_NUM;
        tk.value = value;
        goto token_consumed;
    }
    // Failed to consume this token
    return 1;
token_consumed:
    tokens_.push_back(tk);
    str_ += consumed;
    return 0;
}

void differentiator::tokenize() {
    if (str_ == nullptr)
        throw std::runtime_error("Empty expression!");

    std::size_t length = strlen(str_);
    while (*str_ != '\0') {
        // Iterate through the whole expression
        if (get_token_op() && get_token_num() == 1) {
            // Unknown token
            throw std::runtime_error("Failed to tokenize the expression!");
        }
    }

    // Return the string to the normal state
    str_ -= length;

    // Add the empty last token for correct working algorithm
    tokens_.push_back(token());
}

void differentiator::diff() {
    
}

differentiator::~differentiator() {
    if (str_ != nullptr)
        delete[] str_;
}