#pragma once

#include <stdlib.h>

// parser structure
typedef struct {
    mpc_parser_t* number;
    mpc_parser_t* operator;
    mpc_parser_t* expression;
    mpc_parser_t* program;
} alba_parser_t;

// allocate new parser
alba_parser_t* alba_new_parser() {
    alba_parser_t* parser = (alba_parser_t*) malloc(sizeof(alba_parser_t));

    parser->number     = mpc_new("number");
    parser->operator   = mpc_new("operator");
    parser->expression = mpc_new("expression");
    parser->program    = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                          \
        number     : /-?[0-9]+/ ;                                  \
        operator   : '+' | '-' | '*' | '/';                        \
        expression :  <number> | '(' <operator> <expression>+ ')'; \
        lispy      : /^/ <operator> <expression>+ /$/ ;            \
        ",
        parser->number, parser->operator,
        parser->expression, parser->program);

    return parser;
}
// free lispy praser memory
void alba_free_parser(alba_parser_t* parser) {
    mpc_cleanup(4, parser->number, parser->operator,
                   parser->number, parser->program);
}
