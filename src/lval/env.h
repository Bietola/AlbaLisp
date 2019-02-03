#pragma once

#include "core.h"

// environment structure
typedef struct {
    lval_t** syms;
    lval_t** vals;
    int count;
} env_t;

// constructor
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

// add binding
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
