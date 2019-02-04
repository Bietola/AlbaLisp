#pragma once

#include "core.h"

// environment structure
struct env_t {
    lval_t** syms;
    lval_t** vals;
    int count;
};

// constructors
env_t* env_new() {
    env_t* env = malloc(sizeof(env_t*));
    env->syms = NULL;
    env->vals = NULL;
    env->count = 0;
    return env;
}

// destructor
void env_del(env_t* env) {
    free(env);
}

// add binding (symbol-value pair) to environment
void env_add(env_t* env, lval_t* sym, lval_t* val) {
    // allocate fields if non existent
    if (!env->syms)
        env->syms = malloc(sizeof(lval_t**));
    if (!env->vals)
        env->vals = malloc(sizeof(lval_t**));

    // expand memory
    ++(env->count);
    env->syms = realloc(env->syms, sizeof(lval_t*) * env->count);
    env->vals = realloc(env->vals, sizeof(lval_t*) * env->count);

    // insert val
    env->syms[env->count - 1] = sym;
    env->vals[env->count - 1] = val;
}

// find value associated with given symbol and return its value
lval_t* env_find(env_t* e, lval_t* s) {
    assert(s->type == LVAL_SYM && "Trying to evaluate non-symbol as variable!");

    for (int j = 0; j < e->count; ++j) {
        if (strcmp(e->syms[j]->sym, s->sym) == 0)
            return e->vals[j];
    }

    // return if no variable was found
    return lval_nil();
}
