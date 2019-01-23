#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

// main
int main(int argc, char** argv) {
    // create parser
    mpc_parser_t* Number     = mpc_new("number");
    mpc_parser_t* Operator   = mpc_new("operator");
    mpc_parser_t* Expression = mpc_new("expression");
    mpc_parser_t* Lispy      = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                          \
        number     : /-?[0-9]+/ ;                                  \
        operator   : '+' | '-' | '*' | '/';                        \
        expression :  <number> | '(' <operator> <expression>+ ')'; \
        lispy      : /^/ <expression> /$/ ;                        \
        ",
        Number, Operator, Expression, Lispy);

    // initializing REPL
    puts("AlbaLisp v0.0.1");
    puts("A toy language by Stefano Montesi");

    puts("initializing REPL...");
    puts("REPL ready, press Ctrl-C to terminate terminate it");

    // REPL infinite loop
    while (1) {
        // repl prompt
        char* input = readline("alba> ");
        add_history(input);

        // parse input and do output
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // free memory
        free(input);
    }

    // clean up parser
    mpc_cleanup(4, Number, Operator, Number, Lispy);

    return 0;
}
