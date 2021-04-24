#include "expression.h"

using namespace se;

node::node() : tk(), leftChild_ (nullptr), rightChild_ (nullptr) {}

node* node::get_node() {
    return new node;
}

void node::set_token(const token& rhs) {
    tk = rhs;
}

node::~node() {
    if (leftChild_ != nullptr)
        delete leftChild_;
    if (rightChild_ != nullptr)
        delete rightChild_;
}

node* node::copy() {
    return copy(this);
}

node* node::copy(node* nd) {
    if (nd == nullptr)
        return nd;

    node* new_nd = get_node();
    new_nd->set_token(nd->tk);
    new_nd->leftChild_ = copy(nd->leftChild_);
    new_nd->rightChild_ = copy(nd->rightChild_);

    return new_nd;
}

expression::expression() : str_ (new char[max_str_size]), tree_ (nullptr), exp_counter_ (0), simpled_ (false) {}

void expression::set_string(const char* input_str) {
    str_ = strcpy(str_, input_str);
}

int expression::get_token_op() {
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

int expression::get_token_num() {
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

int expression::get_token_var() {
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

void expression::tokenize() {
    if (str_ == nullptr)
        throw std::runtime_error("Empty expression/variable!");

    tokens_.reserve(128);
    std::size_t length = strlen(str_);
    while (*str_ != '\0') {
        // Iterate through the whole expression
        // Skip spaces
        while (isspace(*str_))
            str_++;
        
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

// Grammar for recursive descent:
// build_tree ::= getPlusMinus'\0'
// getPlusMinus ::= getMulDiv{'+'/'-' getMulDiv}*
// getMulDiv ::= getPow {'*'/'/' getPow}*
// getPow ::= getMathFunc {'^' getMathFunc}
// getMathFunc ::= ('sin' | 'cos' | 'ln' | 'tg' | 'ctg') '(' getPlusMinus ')' | getBrackets
// getBrackets ::= getNumVar | '(' getPlusMinus ')'
// getNumVar ::= [0-9]*

void expression::build_tree(const char* input_str) {
    tokenize();
    tree_ = getPlusMinus();
    if (tokens_[exp_counter_++].type != token_type::TOKEN_NOPE)
        throw std::runtime_error("Unknown syntax error! Expected the end of the expression!\n");
}

node* expression::getPlusMinus() {
    node* nd = nullptr;
    
    nd = getMulDiv();
    token& tk = tokens_[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && ((tk.op_id == token_op_id::OP_PLUS) || (tk.op_id == token_op_id::OP_MINUS))) {
        // If we have plus-minus token, run another getPlusMinus
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Initialize children
        extra_nd->leftChild_ = nd;
        extra_nd->rightChild_ = getPlusMinus();

        return extra_nd;
    } else {
        return nd;
    }
}

node* expression::getMulDiv() {
    node* nd = nullptr;
    
    nd = getPow();
    token& tk = tokens_[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && ((tk.op_id == token_op_id::OP_MUL) || (tk.op_id == token_op_id::OP_DIV))) {
        // If we have mul-div token, run another getMulDiv
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Initialize children
        extra_nd->leftChild_ = nd;
        extra_nd->rightChild_ = getMulDiv();

        return extra_nd;
    } else {
        return nd;
    }
}

node* expression::getPow() {
    node* nd = nullptr;
    
    nd = getMathFunc();
    token& tk = tokens_[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && tk.op_id == token_op_id::OP_POW) {
        // If we have pow token, run another getPow
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Initialize children
        extra_nd->leftChild_ = nd;
        extra_nd->rightChild_ = getPow();

        return extra_nd;
    } else {
        return nd;
    }
}

node* expression::getMathFunc() {
    node* nd = nullptr;

    token& tk = tokens_[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && (tk.op_id == token_op_id::OP_LN || tk.op_id == token_op_id::OP_SIN 
    || tk.op_id == token_op_id::OP_COS || tk.op_id == token_op_id::OP_TG || tk.op_id == token_op_id::OP_CTG)) {
        node* tmp = nullptr;
        // If we have math_func token, run another getMathFunc
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Check brackets
        tk = tokens_[exp_counter_++];
        if (tk.type != token_type::TOKEN_OP || tk.op_id != token_op_id::OP_BR_L)
            throw std::runtime_error("Error with mathfunc syntax!");

        tmp = getPlusMinus(); 

        // Check brackets
        tk = tokens_[exp_counter_++];
        if (tk.type != token_type::TOKEN_OP || tk.op_id != token_op_id::OP_BR_R)
            throw std::runtime_error("Error with mathfunc syntax!");
        
        extra_nd->leftChild_ = tmp;

        return extra_nd;
    } else 
        nd = getBrackets();
    
    return nd;
}

node* expression::getBrackets() {
    node* nd = nullptr;

    token& tk = tokens_[exp_counter_];

    // If we have brackets, call getPlusMinus
    if (tk.type == token_type::TOKEN_OP && tk.op_id == token_op_id::OP_BR_L) {
        exp_counter_++;
        nd = getPlusMinus();
        tk = tokens_[exp_counter_];

        // If brackets are not closed, it is a syntax error
        if (tk.type != token_type::TOKEN_OP || tk.op_id != token_op_id::OP_BR_R)
            throw std::runtime_error("Expected closing brackets!");
        
        exp_counter_++;
        return nd;
    } else {
        // We have a number/var
        return getNumVar();
    }
}

node* expression::getNumVar() {
    node* nd = nullptr;
    token& tk = tokens_[exp_counter_];

    if (tk.type == token_type::TOKEN_NUM || tk.type == token_type::TOKEN_VAR) {
        nd = node::get_node();
        nd->set_token(tk);
        exp_counter_++;
    } else
        throw std::runtime_error("Unknown token type in the method getNumVar()!");

    return nd;
}

double expression::calculate(node* nd) {
    double tmp = 0;
    if (nd == nullptr)
        throw std::runtime_error("Sudden empty node while calculating!");
    
    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            return nd->tk.value;
        case token_type::TOKEN_OP:
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                    return calculate(nd->leftChild_) + calculate(nd->rightChild_);
                case token_op_id::OP_MINUS:
                    return calculate(nd->leftChild_) - calculate(nd->rightChild_);
                case token_op_id::OP_MUL:
                    return calculate(nd->leftChild_) * calculate(nd->rightChild_);
                case token_op_id::OP_DIV:
                    tmp = calculate(nd->rightChild_);
                    if (tmp == 0)
                        throw std::logic_error("Divishion by zero while calculating!");

                    return (calculate(nd->leftChild_) / tmp);
                case token_op_id::OP_POW:
                    return pow(calculate(nd->leftChild_), calculate(nd->rightChild_));
                default:
                    throw std::runtime_error("Sudden unknown operation while calculating!");
            }
            break;
        default:
            throw std::runtime_error("Sudden unknown token while calculating!");
    }
}

double expression::calculate() {
    return calculate(tree_);
}

void expression::set_tree(node* nd) {
    if (tree_ != nullptr)
        delete tree_;
    
    tree_ = nd;
    simpled_ = false;
}

node* expression::get_tree() {
    return tree_;
}

node* expression::simplify(node* nd) {
    if (nd == nullptr)
        return nd;

    // Try calculating this tree
    try {
        double value = calculate(nd);

        delete nd;
        nd = node::get_node();
        nd->tk.type = token_type::TOKEN_NUM;
        nd->tk.value = value;

        return nd;
    }
    catch (std::exception& exception) {
        // Failed to calculate this tree, go deeper
    }

    if (nd->tk.type == token_type::TOKEN_OP) {
        node* nd_l = nd->leftChild_;
        node* nd_r = nd->rightChild_;
        node* tmp = nullptr;
        
        switch (nd->tk.op_id) {
            case token_op_id::OP_PLUS:
                // 0 + f = f
                if (nd_l->tk.type == token_type::TOKEN_NUM && nd_l->tk.value == 0) {
                    tmp = nd_r;

                    delete nd->leftChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }
                // f + 0 = 0
                if (nd_r->tk.type == token_type::TOKEN_NUM && nd_r->tk.value == 0) {
                    tmp = nd_l;

                    delete nd->rightChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }

                // Can't simplify this node, go deeper
                nd->leftChild_ = simplify(nd->leftChild_);
                nd->rightChild_ = simplify(nd->rightChild_);
                return nd;
            case token_op_id::OP_MINUS:
                // 0 - C = -C
                if (nd_l->tk.type == token_type::TOKEN_NUM && nd_l->tk.value == 0 && nd_r->tk.type == token_type::TOKEN_NUM) {
                    tmp = nd_r;

                    delete nd->leftChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    tmp->tk.value *= -1;
                    simpled_ = true;
                    return simplify(tmp);
                }
                // f - 0 = 0
                if (nd_r->tk.type == token_type::TOKEN_NUM && nd_r->tk.value == 0) {
                    tmp = nd_l;

                    delete nd->rightChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }

                // Can't simplify this node, go deeper
                nd->leftChild_ = simplify(nd->leftChild_);
                nd->rightChild_ = simplify(nd->rightChild_);
                return nd;
            case token_op_id::OP_MUL:
                // f * 0 = 0 or 0 * f = 0
                if ((nd_l->tk.type == token_type::TOKEN_NUM && nd_l->tk.value == 0) ||
                    (nd_r->tk.type == token_type::TOKEN_NUM && nd_r->tk.value == 0)) {
                    delete nd;

                    tmp = node::get_node();
                    tmp->tk.type = token_type::TOKEN_NUM;
                    tmp->tk.value = 0;
                    simpled_ = true;
                    return simplify(tmp);
                }
                // 1 * f = f;
                if (nd_l->tk.type == token_type::TOKEN_NUM && nd_l->tk.value == 1) {
                    tmp = nd_r;

                    delete nd->leftChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }
                // f * 1 = f;
                if (nd_r->tk.type == token_type::TOKEN_NUM && nd_r->tk.value == 1) {
                    tmp = nd_l;

                    delete nd->rightChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }

                // Can't simplify this node, go deeper
                nd->leftChild_ = simplify(nd->leftChild_);
                nd->rightChild_ = simplify(nd->rightChild_);
                return nd;
            case token_op_id::OP_DIV:
                // f/1 = f
                if (nd->rightChild_->tk.type == token_type::TOKEN_NUM && nd->rightChild_->tk.value == 1) {
                    tmp = nd_l;

                    delete nd->rightChild_;
                    nd->leftChild_ = nullptr;
                    nd->rightChild_ = nullptr;
                    delete nd;

                    simpled_ = true;
                    return simplify(tmp);
                }
                // 0/f = 0
                if (nd->leftChild_->tk.type == token_type::TOKEN_NUM && nd->leftChild_->tk.value == 0) {
                    delete nd;

                    tmp = node::get_node();
                    tmp->tk.type = token_type::TOKEN_NUM;
                    tmp->tk.value = 0;

                    simpled_ = true;
                    return simplify(tmp);
                }
                // Can't simplify this node
                nd->leftChild_ = simplify(nd->leftChild_);
                nd->rightChild_ = simplify(nd->rightChild_);
                return nd;
            default:
                // Can't simplify this node, go deeper
                nd->leftChild_ = simplify(nd->leftChild_);
                nd->rightChild_ = simplify(nd->rightChild_);
                return nd;
        }
    } else {
        // Not the operation, can't be simplified
        return nd;
    }
}

void expression::simplify() {
    do {
        simpled_ = false;
        tree_ = simplify(tree_);
    } while (simpled_ == true);
}

void expression::latexOutput(FILE* file) {
    latex_print(file, tree_);
}

void expression::latex_print_plus(FILE* file, node* nd) {
    latex_print(file, nd->leftChild_);
    fprintf(file, " + ");
    latex_print(file, nd->rightChild_);
}

void expression::latex_print_minus(FILE* file, node* nd) {
    latex_print(file, nd->leftChild_);
    fprintf(file, " - ");
    latex_print(file, nd->rightChild_);
}

void expression::latex_print_mul(FILE* file, node* nd) {
    if (nd->leftChild_->tk.isCommonOp()) {
        fprintf(file, "(");
        latex_print(file, nd->leftChild_);
        fprintf(file, ")");
    } else {
        latex_print(file, nd->leftChild_);
    }

    fprintf(file, " \\cdot ");

    if (nd->rightChild_->tk.isCommonOp()) {
        fprintf(file, "(");
        latex_print(file, nd->rightChild_);
        fprintf(file, ")");
    } else {
        latex_print(file, nd->rightChild_);
    }
}

void expression::latex_print_div(FILE* file, node* nd) {
    fprintf(file, "\\frac{");
    latex_print(file, nd->leftChild_);
    fprintf(file, "}{");
    latex_print(file, nd->rightChild_);
    fprintf(file, "}");
}

void expression::latex_print_pow(FILE* file, node* nd) {
    fprintf(file, "{");
    latex_print(file, nd->leftChild_);
    fprintf(file, "}^{");
    latex_print(file, nd->rightChild_);
    fprintf(file, "}");
}

void expression::latex_print_mathfunc(FILE* file, node* nd, const char* func) {
    fprintf(file, "%s", func);
    fprintf(file, "(");
    latex_print(file, nd->leftChild_);
    fprintf(file, ")");
}

void expression::latex_print(FILE* file, node* nd) {
    if (file == nullptr)
        throw std::runtime_error("Unexpcted empty file while generating LaTeX file!");
    if (nd == nullptr)
        return ;

    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            if (nd->tk.value >= 0)
                fprintf(file, "%lg", nd->tk.value);
            else
                fprintf(file, "(%lg)", nd->tk.value);
            break;
        case token_type::TOKEN_VAR:
            fprintf(file, VAR);
            break;
        case token_type::TOKEN_OP:
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                    latex_print_plus(file, nd);
                    break;
                case token_op_id::OP_MINUS:
                    latex_print_minus(file, nd);
                    break;
                case token_op_id::OP_MUL:
                    latex_print_mul(file, nd);
                    break;
                case token_op_id::OP_DIV:
                    latex_print_div(file, nd);
                    break;
                case token_op_id::OP_POW:
                    latex_print_pow(file, nd);
                    break;
                case token_op_id::OP_SIN:
                    latex_print_mathfunc(file, nd, "sin");
                    break;
                case token_op_id::OP_COS:
                    latex_print_mathfunc(file, nd, "cos");
                    break;
                case token_op_id::OP_TG:
                    latex_print_mathfunc(file, nd, "tg");
                    break;
                case token_op_id::OP_CTG:
                    latex_print_mathfunc(file, nd, "ctg");
                    break;
                case token_op_id::OP_LN:
                    latex_print_mathfunc(file, nd, "ln");
                    break;
                default:
                    std::cout << "op_id: " << (int) nd->tk.op_id << std::endl;
                    throw std::runtime_error("Unknown operation token type during generating LaTeX file!");
            }
            break;
        default:
            throw std::runtime_error("Unknown token type during generating LaTeX file!");
    }
}

expression::~expression() {
    if (tree_ != nullptr)
        delete tree_;

    delete[] str_;
}