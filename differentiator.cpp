#include "differentiator.h"

using namespace se;

differentiator::differentiator() : str_ (new char[max_str_size]), expression_() {}

void differentiator::set_expression(const char* input_str) {
    strcpy(str_, input_str);
    tokenize(); // first tokenize the expression
    expression_.build_tree(tokens_); // built the expression tree
    //std::cout << "Value: " << expression_.calculate() << std::endl;
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

int differentiator::get_token_var() {
    int consumed = 0;
    token tk;

    sscanf(str_, VAR "%n", &consumed);
    if (consumed > 0) {
        tk.type = token_type::TOKEN_VAR;
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
        throw std::runtime_error("Empty expression/variable!");

    std::size_t length = strlen(str_);
    while (*str_ != '\0') {
        // Iterate through the whole expression
        if (get_token_op() && get_token_num() && get_token_var() == 1) {
            // Unknown token
            throw std::runtime_error("Failed to tokenize the expression!");
        }
    }

    // Return the string to the normal state
    str_ -= length;

    // Add the empty last token for correct working algorithm
    tokens_.push_back(token());
}

node* differentiator::diff_exp(node* nd) {
    node* new_nd = nullptr;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;

    if (nd == nullptr)
        throw std::runtime_error("Unexpected empty node while differentiating!");

    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            // (C)' = 0
            new_nd = node::get_node();
            new_nd->tk.type = token_type::TOKEN_NUM;
            new_nd->tk.value = 0;
            break;
        case token_type::TOKEN_VAR:
            // (x)' = 1
            new_nd = node::get_node();
            new_nd->tk.type = token_type::TOKEN_NUM;
            new_nd->tk.value = 1;
            break;
        case token_type::TOKEN_OP:
            // Operations derivate
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                case token_op_id::OP_MINUS:
                    // (f+g)' = f' + g'
                    new_nd = node::get_node();
                    new_nd->set_token(nd->tk);

                    new_nd->leftChild_ = diff_exp(nd->leftChild_);
                    new_nd->rightChild_ = diff_exp(nd->rightChild_);
                    break;
                case token_op_id::OP_MUL:
                    // (fg)' = f'g + fg'
                    new_nd = node::get_node();
                    new_nd->tk.type = token_type::TOKEN_OP;
                    new_nd->tk.op_id = token_op_id::OP_PLUS;

                    lnew_nd = node::get_node();
                    lnew_nd->tk.type = token_type::TOKEN_OP;
                    lnew_nd->tk.op_id = token_op_id::OP_MUL;
                    lnew_nd->leftChild_ = diff_exp(nd->leftChild_);
                    lnew_nd->rightChild_ = nd->rightChild_->copy();

                    rnew_nd = node::get_node();
                    rnew_nd->tk.type = token_type::TOKEN_OP;
                    rnew_nd->tk.op_id = token_op_id::OP_MUL;
                    rnew_nd->leftChild_ = nd->leftChild_->copy();
                    rnew_nd->rightChild_ = diff_exp(nd->rightChild_);

                    new_nd->leftChild_ = lnew_nd;
                    new_nd->rightChild_ = rnew_nd;
                    break;
            }
            break;
        default:
            throw std::runtime_error("Unknown token type during differentiating!");
    }

    return new_nd;
}

void differentiator::diff() {
    node* diff_nd = diff_exp(expression_.get_tree());
    diff_expression_.set_tree(diff_nd);
    diff_expression_.simplify();
}

// Generate the output LaTeX file
void differentiator::generateLatex(const char* file_name) {
    FILE* file = fopen(file_name, "w");
    if (file == nullptr)
        throw std::runtime_error("Can't open the LaTeX!");

    // Write LaTeX code for creating a document
    fprintf(file, "\\documentclass[12pt, a4paper]{article}\n");
    fprintf(file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(file, "\\usepackage[russian]{babel}\n");
    fprintf(file, "\\usepackage{hyperref}\n");
    fprintf(file, "\\title{\\textbf{Acram Alpha}}\n");
    fprintf(file, "\\date{Автор: \\href{https://github.com/synthMoza}{\\textbf{synthMoza}}}\n");
    fprintf(file, "\\author{\\emph{Дифференциирование приложений}}\n");
    fprintf(file, "\\begin{document}\n");
    fprintf(file, "\\maketitle\n");
    fprintf(file, "\\center{\\textbf{Утрем нос Стивену Вольфраму!}} \\\\ \n");
    fprintf(file, "\\emph{Исходная функция:}\n");
    // Original function
    fprintf(file, "\\[f(x) = ");
    expression_.latexOutput(file);
    fprintf(file, "\\]\n");
    fprintf(file, "\\emph{Её производная:}\n");
    // Derivated function
    fprintf(file, "\\[f'(x) = [");
    diff_expression_.latexOutput(file);
    fprintf(file, "\\]\n");
    fprintf(file, "\\end{document}\n");

    fclose(file);

}

differentiator::~differentiator() {
    if (str_ != nullptr)
        delete[] str_;
}