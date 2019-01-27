#pragma once

// types of lvals
typedef enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_SEXPR
} LVAL_TYPE;

// types of errors
typedef enum {
    LERR_DIV_ZERO,
    LERR_BAD_NUM,
    LERR_BAD_OP
} LERR_TYPE;

// lval
typedef struct lval_t {
    LVAL_TYPE type; // error or number
    union {
        long num;
        char* err;
        char* sym;
        struct {
            int count;
            struct lval_t** cell;
        };
    };
} lval_t;

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

// lval s-expression constructor
lval_t* lval_sexpr(void) {
    lval_t* v = malloc(sizeof(lval_t));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
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
        case LVAL_SEXPR:
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

// add element to lval containing sexpr
void lval_add(lval_t* sexpr, lval_t* toAdd) {
    assert(sexpr && "trying to add lval to NULL sexpr");

    ++(sexpr->count);
    sexpr->cell = realloc(sexpr->cell, sizeof(lval_t*) * sexpr->count);
    sexpr->cell[sexpr->count - 1] = toAdd;
}

// pop element from s-expression lval
lval_t* lval_pop(lval_t* sexpr, int pos) {
    // assert OOB
    assert(pos >= 0 && pos < sexpr->count &&
           "trying to pop sexpr out of bounds");

    // take
    lval_t* ret = sexpr->cell[pos];

    // shift
    memmove(&sexpr->cell[pos], &sexpr->cell[pos + 1],
            sizeof(lval_t*) * (sexpr->count - pos - 1));

    // decrease size
    --sexpr->count;

    // return
    return ret;
}

// pop element from s-expression and deallocate it [the s-expression]
lval_t* lval_take(lval_t* sexpr, int pos) {
    lval_t* ret = lval_pop(sexpr, pos);
    lval_del(sexpr);
    return ret;
}

// read ast node into an lval
lval_t* lval_read_num(const mpc_ast_t* tree) {
    assert(strstr(tree->tag, "number") && "reading a non-number as a number");

    errno = 0;
    long num = strtol(tree->contents, NULL, 10);
    return errno != ERANGE ? lval_num(num) :
                             lval_err("invalid number");
}

// turn ast into lval
lval_t* lval_read(const mpc_ast_t* tree) {
    // check NULL
    assert(tree && "Reading null ast into lval");

    // turn number
    if (strstr(tree->tag, "number"))
        return lval_read_num(tree);

    // turn symbol
    else if (strstr(tree->tag, "symbol"))
        return lval_sym(tree->contents);

    // turn sexpr OR root of program (treated equally for now)
    else if (strcmp(tree->tag, ">") == 0 ||
             strstr(tree->tag, "sexpr")) {
        // create sexpr w/ all children of ast
        lval_t* ret = lval_sexpr();
        for (int j = 0; j < tree->children_num; ++j) {
            mpc_ast_t* child = tree->children[j];
            // filter useless information
            if (strcmp(child->contents, "(") == 0) continue;
            if (strcmp(child->contents, ")") == 0) continue;
            if (strcmp(child->tag,  "regex") == 0) continue;
            // read and add child to sexpr
            lval_add(ret, lval_read(child));
        }
        // return
        return ret;
    }

    // cannot turn ast into any lval
    else
        assert(0 && "trying to read malformed ast into lval");
}

// print atomic lval
void lval_print_expr(const lval_t*, char, char); // forward declaration
void lval_print(const lval_t* v) {
    assert (v && "trying to print NULL lval");

    switch (v->type) {
        case LVAL_NUM   : printf("%li", v->num); break;
        case LVAL_ERR   : printf("%s",  v->err); break;
        case LVAL_SYM   : printf("%s",  v->sym); break;
        case LVAL_SEXPR : lval_print_expr(v, '(', ')'); break;
        default         : assert(0 && "trying to print lval of unknown type");
    }
}
void lval_println(const lval_t* v) { lval_print(v); putchar('\n'); }

// print expr lval
void lval_print_expr(const lval_t* v, char open, char close) {
    assert(v && "tyring to print NULL lval");
    assert(v->type == LVAL_SEXPR && "tyring to print atomic lval as expr lval");

    putchar(open);

    for (int j = 0; j < v->count; ++j) {
        if (j != 0) putchar(' ');
        lval_print(v->cell[j]);
    }

    putchar(close);
}
