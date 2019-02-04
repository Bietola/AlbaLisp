#pragma once

#include <stdio.h>
#include <string.h>

#include "mpc.h"

#include "core.h"
#include "env.h"
#include "builtin.h"

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

// evaluate s-expression
lval_t* lval_eval(env_t*, lval_t*); // forward declaration
lval_t* lval_eval_sexpr(env_t* e, lval_t* v) {
    // evaluate children (apart from symbol)
    for (int j = 0; j < v->count; ++j) {
        v->cell[j] = lval_eval(e, v->cell[j]);
    }

    // propagate errors
    for (int j = 0; j < v->count; ++j) {
        if (v->cell[j]->type == LVAL_ERR)
            return lval_take(v, j);
    }

    // 0 elements: evaluate to itself
    if (v->count == 0) return v;

    // 1 element: take it (eliminating parentheses)
    if (v->count == 1) return lval_take(v, 0);

    // 2 or more: pop first as symbol
    lval_t* sym = lval_pop(v, 0);

    // cleanup before returning error
    #define errclean() lval_del(sym); lval_del(v);

    // ensure it is actually a callable
    //  TODO: only checks and works with builtins for now
    if (sym->type != LVAL_BUILTIN) {
        errclean();
        return lval_err("sexpr needs to have a callable as its first element");
    }

    // evaluate expression using callable
    //  NB: only builtins work for now
    return sym->builtin(e, v);
}

// evaluate lval
lval_t* lval_eval(env_t* e, lval_t* v) {
    assert(v && "trying to evaluate NULL lval");

    // atomic expressions
    switch (v->type) {
        case LVAL_NUM: case LVAL_ERR: case LVAL_QEXPR:
            return v;
        case LVAL_SYM:
            // return associated environment value
            // (or nil if not present)
            return env_find(e, v);
        case LVAL_SEXPR:
            return lval_eval_sexpr(e, v);
        default:
            assert(0 && "trying to evaluate lval of unknown type");
    }
}
