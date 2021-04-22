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

expression::expression() : tree_ (nullptr), exp_counter_ (0) {}

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
// getPlusMinus ::= getNum{'+'/'-' getNum}*

void expression::build_tree(const vector<token>& tokens) {
    tree_ = getPlusMinus(tokens);

    if (tokens.size() != exp_counter_)
        throw std::runtime_error("Unknown syntax error! Expected the end of the expression!\n");
}

node* expression::getPlusMinus(const vector<token>& tokens) {
    node* nd = nullptr;
    
    nd = getNum(tokens);
    token& tk = tokens[exp_counter_++];
    if (tk.type == token_type::TOKEN_OP && ((tk.op_id == token_op_id::OP_PLUS) || (tk.op_id == token_op_id::OP_MINUS))) {
        // If we have plus-minus token, run another getPlusMinus
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

node* expression::getNum(const vector<token>& tokens) {
    node* nd = nullptr;
    token& tk = tokens[exp_counter_];

    switch(tk.type) {
        case token_type::TOKEN_NUM:
            nd = node::get_node();
            nd->set_token(tk);
            exp_counter_++;
            break;
        default:
            throw std::runtime_error("Unknown token type in the method getNum()!");
    }

    return nd;
}

double expression::calculate(node* nd, char var, double value) {
    double tmp = 0;
    if (nd == nullptr)
        throw std::runtime_error("Sudden empty node while calculating!");
    
    switch (nd->tk.type) {
        case token_type::TOKEN_NUM:
            return nd->tk.value;
        case token_type::TOKEN_OP:
            switch (nd->tk.op_id) {
                case token_op_id::OP_PLUS:
                    return calculate(nd->leftChild_, var, value) + calculate(nd->rightChild_, var, value);
                case token_op_id::OP_MINUS:
                    return calculate(nd->leftChild_, var, value) - calculate(nd->rightChild_, var, value);
                case token_op_id::OP_MUL:
                    return calculate(nd->leftChild_, var, value) * calculate(nd->rightChild_, var, value);
                case token_op_id::OP_DIV:
                    tmp = calculate(nd->rightChild_, var, value);
                    if (tmp == 0)
                        throw std::logic_error("Divishion by zero while calculating!");

                    return (calculate(nd->leftChild_, var, value) / tmp);
                default:
                    throw std::runtime_error("Sudden unknown operation while calculating!");
            }
            break;
        default:
            throw std::runtime_error("Sudden unknown toke while calculating!");
    }
}

double expression::calculate(char var, double value) {
    return calculate(tree_, var, value);
}

node* expression::get_tree() {
    return tree_;
}

expression::~expression() {
    if (tree_ != nullptr)
        delete tree_;
}