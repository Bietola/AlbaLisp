#pragma once

#include <stdlib.h>

// parser structure
typedef struct {
    mpc_parser_t* number;
    mpc_parser_t* symbol;
    mpc_parser_t* expr;
    mpc_parser_t* sexpr;
    mpc_parser_t* program;
} alba_parser_t;

// allocate new parser for alba lisp
alba_parser_t* alba_new_parser() {
    alba_parser_t* parser = (alba_parser_t*) malloc(sizeof(alba_parser_t));

    parser->number  = mpc_new("number");
    parser->symbol  = mpc_new("symbol");
    parser->sexpr   = mpc_new("sexpr");
    parser->expr    = mpc_new("expr");
    parser->program = mpc_new("program");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                        \
        number  : /-?[0-9]+/;                    \
        symbol  : '+' | '-' | '*' | '/';         \
        sexpr   : '(' <expr>* ')';               \
        expr    : <number> | <symbol> | <sexpr>; \
        program : /^/ <expr>* /$/;               \
        ",
        parser->number, parser->symbol,
        parser->sexpr, parser->expr,
        parser->program);

    return parser;
}

// free alba lisp parser
void alba_free_parser(alba_parser_t* parser) {
    mpc_cleanup(5, parser->number, parser->symbol,
                   parser->sexpr, parser->expr,
                   parser->program);
}
