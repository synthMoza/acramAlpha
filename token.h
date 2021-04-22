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
    // TODO Add variables and constants
    enum class token_type {
        TOKEN_NOPE, TOKEN_OP, TOKEN_NUM, TOKEN_COUNT
    };
    // Struct that represents the token for expressions
    struct token {
        token_type type;
        union {
            token_op_id op_id;
            double value;
        };

        token() : type (), value () {}
    };
}