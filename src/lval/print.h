#pragma once

#include "core.h"
#include "env.h"

/********/
/* lval */
/********/

// print atomic lval
void lval_print_expr(const lval_t*, char, char); // forward declaration
void lval_print(const lval_t* v) {
    assert (v && "trying to print NULL lval");

    switch (v->type) {
        case LVAL_NUM   : printf("%li", v->num); break;
        case LVAL_ERR   : printf("%s",  v->err); break;
        case LVAL_SYM   : printf("%s",  v->sym); break;
        case LVAL_SEXPR : lval_print_expr(v, '(', ')'); break;
        case LVAL_QEXPR : lval_print_expr(v, '{', '}'); break;
        default         : assert(0 && "trying to print lval of unknown type");
    }
}
void lval_println(const lval_t* v) { lval_print(v); putchar('\n'); }

// print lval containing a s-expression
void lval_print_expr(const lval_t* v, char open, char close) {
    assert(v && "tyring to print NULL lval");
    assert((v->type == LVAL_SEXPR || v->type == LVAL_QEXPR) &&
           "tyring to print atomic lval as expr lval");

    putchar(open);

    for (int j = 0; j < v->count; ++j) {
        if (j != 0) putchar(' ');
        lval_print(v->cell[j]);
    }

    putchar(close);
}

/*******/
/* env */
/*******/

// print environment
void env_print(env_t* env) {
    if (!env) {
        puts("{empty}");
        return;
    }

    for (int j = 0; j < env->count; ++j) {
        printf("%s : ", env->names[j]);
        lval_print(env->bindings[j]);
    }
}
void env_println(env_t* env) {
    env_print(env);
    putchar('\n');
}
