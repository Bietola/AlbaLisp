#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

// TODO: make these not global
mpc_parser_t* Number;
mpc_parser_t* Operator;
mpc_parser_t* Expression;
mpc_parser_t* Lispy;

// create parser
mpc_parser_t* lispy_new_parser() {
    Number     = mpc_new("number");
    Operator   = mpc_new("operator");
    Expression = mpc_new("expression");
    Lispy      = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                          \
        number     : /-?[0-9]+/ ;                                  \
        operator   : '+' | '-' | '*' | '/';                        \
        expression :  <number> | '(' <operator> <expression>+ ')'; \
        lispy      : /^/ <operator> <expression>+ /$/ ;            \
        ",
        Number, Operator, Expression, Lispy);

    return Lispy;
}
// free lispy praser memory
void lispy_free_parser() {
    mpc_cleanup(4, Number, Operator, Number, Lispy);
}

// print lispy ast with needed information
void lispy_print_ast(mpc_ast_t* ast, int depth) {
    if (ast) {
        for (int j = 0; j < depth; ++j) {
            printf("    ");
        }
        printf("%i: |%s| [%s]\n", depth, ast->tag, ast->contents);

        for (int j = 0; j < ast->children_num; ++j) {
            lispy_print_ast(ast->children[j], depth + 1);
        }
    }
}

// evaulate lispy operation
long lispy_eval_op(char op, long rhs, long lhs) {
    switch(op) {
        case '+': return rhs + lhs;
        case '-': return rhs - lhs;
        case '*': return rhs * lhs;
        case '/': return rhs / lhs;
        default : assert(0 && "trying to evaluate invalid lispy operation");
                  return 0;
    }
}

// parse a lispy abstract syntax tree and return its "result"
long lispy_parse_ast(mpc_ast_t* ast) {
    if (ast) {
        // simple expression: number
        if (strstr(ast->tag, "number")) {
            return atoi(ast->contents);
        }
        // complex expression of the form ( <op> <expr>+ )
        else {
            // get operator of expression
            //  NB: skipping the parentheses (or ^) at the beginning
            char op = ast->children[1]->contents[0];
            // accumulate all children using operation
            //  NB: skipping parentheses (or $) at the end
            long result = lispy_parse_ast(ast->children[2]);
            for (int j = 3; j < ast->children_num - 1; ++j) {
                result = lispy_eval_op(op, result, lispy_parse_ast(ast->children[j]));
            }
            return result;
        }
    }
    else {
        assert(0 && "trying to parse null ast node");
        return 0;
    }
}

// main
int main(int argc, char** argv) {
    // create parser
    /*TODO mpc_parser_t* Lispy = */lispy_new_parser();

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
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            long result = lispy_parse_ast(r.output);
            printf("parsing result: %li\n", result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // free input memory
        free(input);
    }

    // clean up parser
    lispy_free_parser();

    return 0;
}
