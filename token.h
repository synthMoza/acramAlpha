#pragma once

#include "vector.h"

namespace se {
    // Enum class that contains values that represent each operation
    enum class token_op_id {
        #define DEF_OP(name, value) OP_##name,
        #include "def_op.h"
        #undef DEF_OP
        OP_COUNT
    };

    // Enum class that contains all token types
    enum class token_type {
        TOKEN_NOPE, TOKEN_OP, TOKEN_NUM, TOKEN_VAR, TOKEN_COUNT
    };
    // Struct that represents the token for expressions
    struct token {
        token_type type;
        union {
            token_op_id op_id; // operation ID
            double value; // number value
        };
    
        token() : type (), value () {}
        // Returns true if the token is one of operations: sin, cos, tg, ctg, ln
        bool isMathFunc() {
            return (type == token_type::TOKEN_OP && (op_id == token_op_id::OP_SIN || op_id == token_op_id::OP_COS || 
                op_id == token_op_id::OP_TG || op_id == token_op_id::OP_CTG || op_id == token_op_id::OP_LN));
        }
        // Returns true if the token is one of the operations: +, -, *, /
        bool isCommonOp() {
            return (type == token_type::TOKEN_OP && (op_id == token_op_id::OP_PLUS || op_id == token_op_id::OP_MINUS || 
                op_id == token_op_id::OP_MUL || op_id == token_op_id::OP_DIV));
        }
    };
}