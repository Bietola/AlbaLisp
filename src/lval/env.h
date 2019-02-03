#pragma once

#include "core.h"

// environment structure
typedef struct {
    const char** names;
    lval_t** bindings;
    int count;
} env_t;

// constructor
env_t* env_new() {
    env_t* env = malloc(sizeof(env_t*));
    env->names = NULL;
    env->bindings = NULL;
    env->count = 0;
    return env;
}

// destructor
void env_del(env_t* env) {
    free(env);
}

// add binding
void env_add(env_t* env, const char* name, lval_t* binding) {
    // allocate fields if non existent
    if (!env->names)
        env->names = malloc(sizeof(const char**));
    if (!env->bindings)
        env->bindings = malloc(sizeof(lval_t**));

    // expand memory
    ++(env->count);
    env->names    = realloc(env->names,    sizeof(const char*) * env->count);
    env->bindings = realloc(env->bindings, sizeof(lval_t*) * env->count);

    // insert binding
    env->names   [env->count - 1] = name;
    env->bindings[env->count - 1] = binding;
}
