#pragma once

// forward declarations
struct env_t;
struct lval_t;
typedef struct env_t env_t;
typedef struct lval_t lval_t;

// type used to store builtin functions
// and associate them with symbols
typedef lval_t* (*builtin_t)(env_t*, lval_t*);

// types of lvals
typedef enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_BUILTIN,
    LVAL_SEXPR,
    LVAL_QEXPR
} LVAL_TYPE;

// types of errors
typedef enum {
    LERR_DIV_ZERO,
    LERR_BAD_NUM,
    LERR_BAD_OP
} LERR_TYPE;

// lval
struct lval_t {
    LVAL_TYPE type; // error or number
    union {
        long num;
        char* err;
        char* sym;
        builtin_t builtin;
        struct {
            int count;
            struct lval_t** cell;
        };
    };
};

// lval number constructor
lval_t* lval_num(long num) {
    lval_t* v = malloc(sizeof(lval_t));
    v->num = num;
    v->type = LVAL_NUM;
    return v;
}

// lval error constructor
lval_t* lval_err(char* err) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(err) + 1);
    strcpy(v->err, err);
    return v;
}

// lval symbol constructor
lval_t* lval_sym(char* sym) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(sym) + 1);
    strcpy(v->sym, sym);
    return v;
}

// lval builtin constructor
lval_t* lval_builtin(builtin_t builtin) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_BUILTIN;
    v->builtin = builtin;
    return v;
}

// free lval memory
void lval_del(lval_t* v) {
    switch (v->type) {
        case LVAL_NUM:
            ; // nothing to deallocate
            break;
        case LVAL_ERR:
            free(v->err);
            break;
        case LVAL_SYM:
            free(v->sym);
            break;
        case LVAL_BUILTIN:
            // pointer to builtin function is non-owning
            break;
        case LVAL_SEXPR: case LVAL_QEXPR:
            for (int j = 0; j < v->count; ++j) {
                lval_del(v->cell[j]);
            }
            free(v->cell);
            break;
        default:
            assert(0 && "trying to deallocate malformed lval");
    }

    free(v);
}

// lval s-expression constructor
lval_t* lval_sexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

// lval q-expression constructor
lval_t* lval_qexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

// lval nil constructor
//  NB: just returns an empty qexpr for now
lval_t* lval_nil(void) {
    return lval_qexpr();
}
