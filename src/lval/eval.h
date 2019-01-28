#pragma once

#include <stdio.h>
#include <string.h>

#include "mpc.h"

#include "core.h"

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

// evaluate builtin operator
lval_t* builtin_op(const char* op, lval_t* v) {
    assert(op  && "trying to evaluate NULL builtin sym");
    assert(v   && "trying to evaluate builtin  on NULL expression");
    assert(v->count != 0 &&
           "trying to evaluate builtin op on empty expression");

    // ensure all arguments are numbers
    for (int j = 0; j < v->count; ++j) {
        if (v->cell[j]->type != LVAL_NUM) {
            lval_del(v);
            return lval_err("cannot operate on non-number!");
        }
    }

    // take first element
    lval_t* acc = lval_pop(v, 0);

    // unary minus
    if (v->count == 0 && strcmp(op, "-") == 0) {
        acc->num = -acc->num;
        lval_del(v);
        return acc;
    }

    // more than one element
    while (v->count) {
        lval_t* nv = lval_pop(v, 0);

        // match operator
        if      (strcmp(op, "+") == 0) acc->num += nv->num;
        else if (strcmp(op, "-") == 0) acc->num -= nv->num;
        else if (strcmp(op, "*") == 0) acc->num *= nv->num;
        else if (strcmp(op, "/") == 0) {
            if (nv->num == 0) {
                lval_del(acc); lval_del(nv);
                acc = lval_err("cannot perform division by 0!");
                break;
            }
            else
                acc->num /= nv->num;
        }
    }

    lval_del(v); return acc;
}

// evaluate s-expression
lval_t* lval_eval(lval_t*); // forward declaration
lval_t* lval_eval_sexpr(lval_t* v) {
    // evaluate children
    for (int j = 0; j < v->count; ++j) {
        v->cell[j] = lval_eval(v->cell[j]);
    }

    // propagate errors
    for (int j = 0; j < v->count; ++j) {
        if (v->cell[j]->type == LVAL_ERR)
            return lval_take(v, j);
    }

    // 0 elements: evaluate to itself
    if (v->count == 0) return v;

    // 1 element: take it
    if (v->count == 1) return lval_take(v, 0);

    // 2 or more: pop first as symbol
    lval_t* sym = lval_pop(v, 0);

    // ensure it is actually a symbol
    if (sym->type != LVAL_SYM) {
        lval_del(sym); lval_del(v);
        return lval_err("sexpr needs to have symbol as its first element");
    }

    // evaluate expression using symbol
    return builtin_op(sym->sym, v);
}

// evaluate lval
lval_t* lval_eval(lval_t* v) {
    assert(v && "trying to evaluate NULL lval");

    // atomic expressions
    switch (v->type) {
        case LVAL_NUM: case LVAL_SYM: case LVAL_ERR: case LVAL_QEXPR:
            return v;
        case LVAL_SEXPR:
            return lval_eval_sexpr(v);
        default:
            assert(0 && "trying to evaluate lval of unknown type");
    }
}