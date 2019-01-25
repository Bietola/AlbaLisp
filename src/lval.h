#pragma once

//
typedef enum {LVAL_NUM, LVAL_ERR};

//
enum {LERR_DIV_ZERO, LERR_BAD_NUM, LERR_BAD_OP};

//
typedef struct {
    int type; // error or number
    union {
        long num;
        int err;
    };
} lval_t;

// lval constructor
lval_t lval_num(long num) {
    lval_t v;
    v.num = num;
    v.type = LVAL_NUM;
    return v;
}

// lval constructor
lval_t lval_err(int err) {
    lval_t v;
    v.err = err;
    v.type = LVAL_ERR;
    return v;
}

// printing facilities
void lval_print(const lval_t* v) {
    assert(v && "trying to print NULL lval.");

    switch(v->type) {
        case LVAL_NUM:
            printf("%li", v->num);
            break;
        case LVAL_ERR:
            switch(v->err) {
                case LERR_DIV_ZERO:
                    printf("Error: division by 0!");
                    break;
                case LERR_BAD_OP:
                    printf("Error: invalid operator!");
                    break;
                case LERR_BAD_NUM:
                    printf("Error: invalid number!");
                    break;
                default:
                    assert(0 && "Assertion: trying to print malformed error.");
            }
            break;
        default:
            assert(0 && "Assertion: trying to print lval of malformed type.");
    }
}
void lval_println(const lval_t* v) { lval_print(v); putchar('\n'); }
