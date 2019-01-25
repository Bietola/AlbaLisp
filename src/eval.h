#pragma once

#include <stdio.h>
#include <string.h>

#include "mpc.h"

#include "lval.h"

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
lval_t alba_eval_op(char op, lval_t rhs, lval_t lhs) {
    // propagate errors
    if (lhs.type == LVAL_ERR) return lhs;
    if (rhs.type == LVAL_ERR) return rhs;

    // operations on numbers
    long rhn = rhs.num;
    long lhn = lhs.num;
    switch (op) {
        case '+': return lval_num(rhn + lhn);
        case '-': return lval_num(rhn - lhn);
        case '*': return lval_num(rhn * lhn);
        case '/':
            return lhn == 0 ? lval_err(LERR_DIV_ZERO)
                            : lval_num(rhn / lhn);
        default : return lval_err(LERR_BAD_OP);
    }
}

// parse an alba abstract syntax tree and return its "result"
lval_t alba_eval(mpc_ast_t* ast) {
    if (ast) {
        // simple expression: number
        if (strstr(ast->tag, "number")) {
            errno = 0;
            long result = strtol(ast->contents, NULL, 10);
            return errno != ERANGE ? lval_num(result) : lval_err(LERR_BAD_NUM); // handle bad number error
        }
        // complex expression of the form ( <op> <expr>+ )
        else {
            // get operator of expression
            //  NB: skipping the parentheses (or ^) at the beginning
            char op = ast->children[1]->contents[0];
            // accumulate all children using operation
            //  NB: skipping parentheses (or $) at the end
            lval_t result = alba_eval(ast->children[2]);
            for (int j = 3; j < ast->children_num - 1; ++j) {
                result = alba_eval_op(op, result, alba_eval(ast->children[j]));
            }
            return result;
        }
    }
    else {
        assert(0 && "trying to parse null ast node");
        return lval_num(0);
    }
}

