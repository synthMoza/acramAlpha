#include "differentiator.h"

using namespace se;

void differentiator::set_expression(const char* input_str) {
    expression_.set_string(input_str);
    expression_.build_tree(input_str); // built the expression tree
}

node* differentiator::diff_num(node* nd) {
    // (C)' = 0
    node* new_nd;

    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_NUM;
    new_nd->tk.value = 0;

    return new_nd;
}

node* differentiator::diff_var(node* nd) {
    // (x)' = 1
    node* new_nd;
    
    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_NUM;
    new_nd->tk.value = 1;

    return new_nd;
}

node* differentiator::diff_plus_minus(node* nd) {
    // (f+g)' = f' + g'
    node* new_nd;
    
    new_nd = node::get_node();
    new_nd->set_token(nd->tk);

    new_nd->leftChild_ = diff_exp(nd->leftChild_);
    new_nd->rightChild_ = diff_exp(nd->rightChild_);

    return new_nd;
}

node* differentiator::diff_mul(node* nd) {
    // (fg)' = f'g + fg'
    node* new_nd;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;
    
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

    return new_nd;
}

node* differentiator::diff_pow(node* nd) {
    node* new_nd;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;
    node* rrnew_nd = nullptr;
    node* rrrnew_nd = nullptr;
    node* rlnew_nd = nullptr;
    
    if (nd->leftChild_->tk.type == token_type::TOKEN_NUM) {
        // (a^x)' = a^x * ln(a)
        new_nd = node::get_node();
        new_nd->tk.type = token_type::TOKEN_OP;
        new_nd->tk.op_id = token_op_id::OP_MUL;

        lnew_nd = nd->copy();

        rnew_nd = node::get_node();
        rnew_nd->tk.type = token_type::TOKEN_OP;
        rnew_nd->tk.op_id = token_op_id::OP_LN;
        rnew_nd->leftChild_ = node::get_node();
        rnew_nd->leftChild_->set_token(nd->leftChild_->tk);

        new_nd->leftChild_ = lnew_nd;
        new_nd->rightChild_ = rnew_nd;

        return new_nd;
    }

    if (nd->rightChild_->tk.type == token_type::TOKEN_NUM) {
        // (x^a)' = a*x^(a-1)
        new_nd = node::get_node();
        new_nd->tk.type = token_type::TOKEN_OP;
        new_nd->tk.op_id = token_op_id::OP_MUL;

        lnew_nd = nd->rightChild_->copy();

        rnew_nd = nd->copy();
        rnew_nd->rightChild_->tk.value -= 1;

        new_nd->leftChild_ = lnew_nd;
        new_nd->rightChild_ = rnew_nd;

        return new_nd;
    }

    // (f^g)' = f^g * (g'ln(f)+g/f)
    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_MUL;

    lnew_nd = nd->copy();
    
    rnew_nd = node::get_node();
    rnew_nd->tk.type = token_type::TOKEN_OP;
    rnew_nd->tk.op_id = token_op_id::OP_PLUS;

    rrnew_nd = node::get_node();
    rrnew_nd->tk.type = token_type::TOKEN_OP;
    rrnew_nd->tk.op_id = token_op_id::OP_MUL;

    rrrnew_nd = node::get_node();
    rrrnew_nd->tk.type = token_type::TOKEN_OP;
    rrrnew_nd->tk.op_id = token_op_id::OP_DIV;
    rrrnew_nd->leftChild_ = nd->rightChild_->copy();
    rrrnew_nd->rightChild_ = nd->leftChild_->copy();

    rrnew_nd->rightChild_ = rrrnew_nd;
    rrnew_nd->leftChild_ = diff_exp(nd->leftChild_);

    rlnew_nd = node::get_node();
    rlnew_nd->tk.type = token_type::TOKEN_OP;
    rlnew_nd->tk.op_id = token_op_id::OP_MUL;
    
    rlnew_nd->leftChild_ = diff_exp(nd->rightChild_);
    rlnew_nd->rightChild_ = node::get_node();
    rlnew_nd->rightChild_->tk.type = token_type::TOKEN_OP;
    rlnew_nd->rightChild_->tk.op_id = token_op_id::OP_LN;
    rlnew_nd->rightChild_->leftChild_ = nd->leftChild_->copy();
    
    rnew_nd->leftChild_ = rlnew_nd;
    rnew_nd->rightChild_ = rrnew_nd;
    new_nd->leftChild_ = lnew_nd;
    new_nd->rightChild_ = rnew_nd;

    return new_nd;
}

node* differentiator::diff_sin(node* nd) {
    // (sin(f))' = cos(f) * f'
    node* new_nd;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;
    
    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_MUL;

    lnew_nd = node::get_node();
    lnew_nd->tk.type = token_type::TOKEN_OP;
    lnew_nd->tk.op_id = token_op_id::OP_COS;
    lnew_nd->leftChild_ = nd->leftChild_->copy();
    rnew_nd = diff_exp(nd->leftChild_);

    new_nd->leftChild_ = lnew_nd;
    new_nd->rightChild_ = rnew_nd;

    return new_nd;
}

node* differentiator::diff_cos(node* nd) {
    // (cos(f))' = -sin(f) * f'
    node* new_nd;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;
    
    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_MUL;

    rnew_nd = diff_exp(nd->leftChild_);

    lnew_nd = node::get_node();
    lnew_nd->tk.type = token_type::TOKEN_OP;
    lnew_nd->tk.op_id = token_op_id::OP_MUL;
    lnew_nd->leftChild_ = node::get_node();
    lnew_nd->leftChild_->tk.type = token_type::TOKEN_NUM;
    lnew_nd->leftChild_->tk.value = -1;
    lnew_nd->rightChild_= node::get_node();
    lnew_nd->rightChild_->tk.type = token_type::TOKEN_OP;
    lnew_nd->rightChild_->tk.op_id = token_op_id::OP_SIN;
    lnew_nd->rightChild_->leftChild_ = nd->leftChild_->copy();

    new_nd->leftChild_ = lnew_nd;
    new_nd->rightChild_ = rnew_nd;

    return new_nd;
}

node* differentiator::diff_div(node* nd) {
    // (f/g)' = (f'g-gf')/g^2
    node* new_nd;
    node* lnew_nd = nullptr;
    node* rnew_nd = nullptr;
    
    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_DIV;

    lnew_nd = node::get_node();
    lnew_nd->tk.type = token_type::TOKEN_OP;
    lnew_nd->tk.op_id = token_op_id::OP_MINUS;
    
    lnew_nd->leftChild_ = node::get_node();
    lnew_nd->leftChild_->tk.type = token_type::TOKEN_OP;
    lnew_nd->leftChild_->tk.op_id = token_op_id::OP_MUL;
    lnew_nd->leftChild_->leftChild_ = diff_exp(nd->leftChild_);
    lnew_nd->leftChild_->rightChild_ = nd->rightChild_->copy();

    lnew_nd->rightChild_ = node::get_node();
    lnew_nd->rightChild_->tk.type = token_type::TOKEN_OP;
    lnew_nd->rightChild_->tk.op_id = token_op_id::OP_MUL;
    lnew_nd->rightChild_->leftChild_ = nd->leftChild_->copy();
    lnew_nd->rightChild_->rightChild_ = diff_exp(nd->rightChild_);

    rnew_nd = node::get_node();
    rnew_nd->tk.type = token_type::TOKEN_OP;
    rnew_nd->tk.op_id = token_op_id::OP_POW;
    rnew_nd->leftChild_ = nd->rightChild_->copy();
    rnew_nd->rightChild_ = node::get_node();
    rnew_nd->rightChild_->tk.type = token_type::TOKEN_NUM;
    rnew_nd->rightChild_->tk.value = 2;

    new_nd->leftChild_ = lnew_nd;
    new_nd->rightChild_ = rnew_nd;

    return new_nd;
}

node* differentiator::diff_ln(node* nd) {
    // (ln(f))' = f'/f
    node* new_nd = nullptr;

    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_DIV;
    new_nd->leftChild_ = diff_exp(nd->leftChild_);
    new_nd->rightChild_ = nd->leftChild_->copy();

    return new_nd;
}

node* differentiator::diff_tg(node* nd) {
    // (tg(f))' = f' / cos^2(f)
    node* new_nd = nullptr;
    node* rnew_nd = nullptr;
    node* lnew_nd = nullptr;

    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_DIV;
    
    lnew_nd = diff_exp(nd->leftChild_);

    rnew_nd = node::get_node();
    rnew_nd->tk.type = token_type::TOKEN_OP;
    rnew_nd->tk.op_id = token_op_id::OP_POW;
    
    rnew_nd->leftChild_ = node::get_node();
    rnew_nd->leftChild_->tk.type = token_type::TOKEN_OP;
    rnew_nd->leftChild_->tk.op_id = token_op_id::OP_COS;
    rnew_nd->leftChild_->leftChild_ = nd->leftChild_->copy();
    rnew_nd->rightChild_ = node::get_node();
    rnew_nd->rightChild_->tk.type = token_type::TOKEN_NUM;
    rnew_nd->rightChild_->tk.value = 2;

    new_nd->rightChild_ = rnew_nd;
    new_nd->leftChild_ = lnew_nd;

    return new_nd;
}

node* differentiator::diff_ctg(node* nd) {
    // (ctg(f))' = -1 * f' / sin^2(f)
    node* new_nd = nullptr;
    node* rnew_nd = nullptr;
    node* lnew_nd = nullptr;

    new_nd = node::get_node();
    new_nd->tk.type = token_type::TOKEN_OP;
    new_nd->tk.op_id = token_op_id::OP_MUL;
    new_nd->leftChild_ = node::get_node();
    new_nd->leftChild_->tk.type = token_type::TOKEN_NUM;
    new_nd->leftChild_->tk.value = -1;

    new_nd->rightChild_ = node::get_node();
    new_nd->rightChild_->tk.type = token_type::TOKEN_OP;
    new_nd->rightChild_->tk.op_id = token_op_id::OP_DIV;
    
    lnew_nd = diff_exp(nd->leftChild_);

    rnew_nd = node::get_node();
    rnew_nd->tk.type = token_type::TOKEN_OP;
    rnew_nd->tk.op_id = token_op_id::OP_POW;
    
    rnew_nd->leftChild_ = node::get_node();
    rnew_nd->leftChild_->tk.type = token_type::TOKEN_OP;
    rnew_nd->leftChild_->tk.op_id = token_op_id::OP_SIN;
    rnew_nd->leftChild_->leftChild_ = nd->leftChild_->copy();
    rnew_nd->rightChild_ = node::get_node();
    rnew_nd->rightChild_->tk.type = token_type::TOKEN_NUM;
    rnew_nd->rightChild_->tk.value = 2;

    new_nd->rightChild_->rightChild_ = rnew_nd;
    new_nd->rightChild_->leftChild_ = lnew_nd;

    return new_nd;
}

node* differentiator::diff_exp(node* nd) {
    node* new_nd = nullptr;

    if (nd == nullptr)
        throw std::runtime_error("Unexpected empty node while differentiating!");

    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            new_nd = diff_num(nd);
            break;
        case token_type::TOKEN_VAR:
            new_nd = diff_var(nd);
            break;
        case token_type::TOKEN_OP:
            // Operations derivate
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                case token_op_id::OP_MINUS:
                    new_nd = diff_plus_minus(nd);
                    break;
                case token_op_id::OP_MUL:
                    new_nd = diff_mul(nd);
                    break;
                case token_op_id::OP_POW:
                    new_nd = diff_pow(nd);
                    break;
                case token_op_id::OP_SIN:
                    new_nd = diff_sin(nd);
                    break;
                case token_op_id::OP_COS:
                    new_nd = diff_cos(nd);
                    break;
                case token_op_id::OP_DIV:
                    new_nd = diff_div(nd);
                    break;
                case token_op_id::OP_TG:
                    new_nd = diff_tg(nd);
                    break;
                case token_op_id::OP_CTG:
                    new_nd = diff_ctg(nd);
                    break;
                case token_op_id::OP_LN:
                    new_nd = diff_ln(nd);
                    break;
                default:
                    throw std::runtime_error("Unknown operation for differentiating!");
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
    fprintf(file, "\\author{\\emph{Дифференциирование функций}}\n");
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
    fprintf(file, "\\[f'(x) = ");
    diff_expression_.latexOutput(file);
    fprintf(file, "\\]\n");
    fprintf(file, "\\end{document}\n");

    fclose(file);

}