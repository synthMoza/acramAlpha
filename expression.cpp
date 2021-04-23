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

expression::expression() : tree_ (nullptr), exp_counter_ (0), simpled_ (false) {}

// Grammar for recursive descent
// TODO Needed at the end:
// getTree ::= getPlusMinus'\0'
// getPlusMinus ::= getMulDiv{'+'/'-' getMulDiv}*
// getMulDiv ::= getPow {'*'/':' getPow}*
// getPow ::= getMathFunc{'^' getMathFunc}
// getMathFunc ::= ('sin' | 'cos' | 'ln' | 'tan' | 'sqrt') '(' getPlusMinus ')' | getBrackets
// getBrackets ::= getNumVar | '(' getPlusMinus ')'
// getNumVar ::= NUMBER | 'x' | CONST

// Now:
// build_tree ::= getPlusMinus'\0'
// getPlusMinus ::= getMulDiv{'+'/'-' getMulDiv}*
// getMulDiv ::= getBrackets {'*'/'/' getBrackets}*
// getBrackets ::= getNumVar | '(' getPlusMinus ')'
// getNumVar ::= [0-9]*

void expression::build_tree(const vector<token>& tokens) {
    tree_ = getPlusMinus(tokens);
    if (tokens[exp_counter_++].type != token_type::TOKEN_NOPE)
        throw std::runtime_error("Unknown syntax error! Expected the end of the expression!\n");
}

node* expression::getPlusMinus(const vector<token>& tokens) {
    node* nd = nullptr;
    
    nd = getMulDiv(tokens);
    token& tk = tokens[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && ((tk.op_id == token_op_id::OP_PLUS) || (tk.op_id == token_op_id::OP_MINUS))) {
        // If we have plus-minus token, run another getPlusMinus
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Initialize children
        extra_nd->leftChild_ = nd;
        extra_nd->rightChild_ = getPlusMinus(tokens);

        return extra_nd;
    } else {
        return nd;
    }
}

node* expression::getMulDiv(const vector<token>& tokens) {
    node* nd = nullptr;
    
    nd = getBrackets(tokens);
    token& tk = tokens[exp_counter_];
    if (tk.type == token_type::TOKEN_OP && ((tk.op_id == token_op_id::OP_MUL) || (tk.op_id == token_op_id::OP_DIV))) {
        // If we have mul-div token, run another getMulDiv
        exp_counter_++;
        // Copy the token
        node* extra_nd = node::get_node();
        extra_nd->set_token(tk);

        // Initialize children
        extra_nd->leftChild_ = nd;
        extra_nd->rightChild_ = getMulDiv(tokens);

        return extra_nd;
    } else {
        return nd;
    }
}

node* expression::getBrackets(const vector<token>& tokens) {
    node* nd = nullptr;

    token& tk = tokens[exp_counter_];

    // If we have brackets, call getPlusMinus
    if (tk.type == token_type::TOKEN_OP && tk.op_id == token_op_id::OP_BR_L) {
        exp_counter_++;
        nd = getPlusMinus(tokens);
        tk = tokens[exp_counter_];

        // If brackets are not closed, it is a syntax error
        if (tk.type != token_type::TOKEN_OP || tk.op_id != token_op_id::OP_BR_R)
            throw std::runtime_error("Expected closing brackets!");
        
        exp_counter_++;
        return nd;
    } else {
        // We have a number/var
        return getNumVar(tokens);
    }
}

node* expression::getNumVar(const vector<token>& tokens) {
    node* nd = nullptr;
    token& tk = tokens[exp_counter_];

    if (tk.type == token_type::TOKEN_NUM || tk.type == token_type::TOKEN_VAR) {
        nd = node::get_node();
        nd->set_token(tk);
        exp_counter_++;
    } else
        throw std::runtime_error("Unknown token type in the method getNumVar()!");

    return nd;
}

double expression::calculate(node* nd, double value) {
    double tmp = 0;
    if (nd == nullptr)
        throw std::runtime_error("Sudden empty node while calculating!");
    
    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            return nd->tk.value;
        case token_type::TOKEN_OP:
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                    return calculate(nd->leftChild_, value) + calculate(nd->rightChild_, value);
                case token_op_id::OP_MINUS:
                    return calculate(nd->leftChild_, value) - calculate(nd->rightChild_, value);
                case token_op_id::OP_MUL:
                    return calculate(nd->leftChild_, value) * calculate(nd->rightChild_, value);
                case token_op_id::OP_DIV:
                    tmp = calculate(nd->rightChild_, value);
                    if (tmp == 0)
                        throw std::logic_error("Divishion by zero while calculating!");

                    return (calculate(nd->leftChild_, value) / tmp);
                default:
                    throw std::runtime_error("Sudden unknown operation while calculating!");
            }
            break;
        default:
            throw std::runtime_error("Sudden unknown token while calculating!");
    }
}

double expression::calculate(double value) {
    return calculate(tree_, value);
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
                break;
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
                break;
            case token_op_id::OP_MUL:
                // f * 0 = 0 or 0 * f = 0
                if ((nd_l->tk.type == token_type::TOKEN_NUM && nd_l->tk.value == 0) ||
                    (nd_r->tk.type == token_type::TOKEN_NUM && nd_r->tk.value == 0)) {
                    delete nd;

                    tmp = node::get_node();
                    tmp->tk.type = token_type::TOKEN_NUM;
                    tmp->tk.value = 0;
                    simpled_ = true;
                    return tmp;
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
                break;
            case token_op_id::OP_DIV:
                // TODO
                return nd;
                break;
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
                    latex_print(file, nd->leftChild_);
                    fprintf(file, " + ");
                    latex_print(file, nd->rightChild_);
                    break;
                case token_op_id::OP_MINUS:
                    latex_print(file, nd->leftChild_);
                    fprintf(file, " - ");
                    latex_print(file, nd->rightChild_);
                    break;
                case token_op_id::OP_MUL:
                    if (nd->leftChild_->tk.type == token_type::TOKEN_OP) {
                        fprintf(file, "(");
                        latex_print(file, nd->leftChild_);
                        fprintf(file, ")");
                    } else {
                        latex_print(file, nd->leftChild_);
                    }
                
                    fprintf(file, " * ");

                    if (nd->rightChild_->tk.type == token_type::TOKEN_OP) {
                        fprintf(file, "(");
                        latex_print(file, nd->rightChild_);
                        fprintf(file, ")");
                    } else {
                        latex_print(file, nd->rightChild_);
                    }
                    break;
                case token_op_id::OP_DIV:
                    fprintf(file, "\\frac{");
                    latex_print(file, nd->leftChild_);
                    fprintf(file, "}{");
                    latex_print(file, nd->rightChild_);
                    fprintf(file, "}");
                    break;
                default:
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
}