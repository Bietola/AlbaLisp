#pragma once

#include "core.h"

// add element to lval containing expr
void lval_add(lval_t* expr, lval_t* toAdd) {
    assert(expr && "trying to add lval to NULL expr");

    ++(expr->count);
    expr->cell = realloc(expr->cell, sizeof(lval_t*) * expr->count);
    expr->cell[expr->count - 1] = toAdd;
}

// pop element from s-expression lval
lval_t* lval_pop(lval_t* expr, int pos) {
    // return NULL if expr is empty
    if (expr->count == 0)
        return NULL;

    // take
    lval_t* ret = expr->cell[pos];

    // shift
    memmove(&expr->cell[pos], &expr->cell[pos + 1],
            sizeof(lval_t*) * (expr->count - pos - 1));

    // decrease size
    --expr->count;

    // return
    return ret;
}

// pop element from s-expression and deallocate it [the s-expression]
lval_t* lval_take(lval_t* expr, int pos) {
    lval_t* ret = lval_pop(expr, pos);
    lval_del(expr);
    return ret;
}
