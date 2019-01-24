#pragma once

#include <stdio.h>
#include <string.h>

#include "mpc.h"

// db print lispy ast with needed information
void alba_print_ast(mpc_ast_t* ast, int depth) {
    if (ast) {
        for (int j = 0; j < depth; ++j) {
            printf("    ");
        }
        printf("%i: |%s| [%s]\n", depth, ast->tag, ast->contents);

        for (int j = 0; j < ast->children_num; ++j) {
            alba_print_ast(ast->children[j], depth + 1);
        }
    }
}

// evaulate lispy operation
long alba_eval_op(char op, long rhs, long lhs) {
    switch(op) {
        case '+': return rhs + lhs;
        case '-': return rhs - lhs;
        case '*': return rhs * lhs;
        case '/': return rhs / lhs;
        default : assert(0 && "trying to evaluate invalid lispy operation");
                  return 0;
    }
}

// parse an alba abstract syntax tree and return its "result"
long alba_eval(mpc_ast_t* ast) {
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
            long result = alba_eval(ast->children[2]);
            for (int j = 3; j < ast->children_num - 1; ++j) {
                result = alba_eval_op(op, result, alba_eval(ast->children[j]));
            }
            return result;
        }
    }
    else {
        assert(0 && "trying to parse null ast node");
        return 0;
    }
}

