#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

#include "parsing.h"
#include "lval/all.h"

// repl loop
void repl() {
    // create parser
    alba_parser_t* parser = alba_new_parser();

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

        // parse program and return "result"
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, parser->program, &r)) {
            lval_t* result = lval_eval(lval_read(r.output));
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // free input memory
        free(input);
    }

    // clean up parser
    alba_free_parser(parser);
}

// MAIN
int main(int argc, char** argv) {
    repl();

    return 0;
}
