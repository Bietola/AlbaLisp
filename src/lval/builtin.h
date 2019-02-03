#pragma once

#include "core.h"

// assertion macro used to return error msg
#define LASSERT(CND, LVL, ERRMSG) \
    if (!(CND)) {                   \
        lval_del(LVL);            \
        return lval_err(ERRMSG);  \
    }

// bounds assertion
#define LASSERT_BOUNDS(LVL, MINB, MAXB, MINMSG, MAXMSG) \
    LASSERT(LVL->count >= MINB, LVL, MINMSG) \
    LASSERT(LVL->count <= MINB, LVL, MAXMSG)


/**********************************************************/
/*          builtin operators and functions               */
/*--------------------------------------------------------*/
/* NB: all builtin functions expect an s-expression       */
/*     as their sole parameter. This expression countains */
/*     every piece of input that the operator requires.   */
/**********************************************************/

/**************************/
/* q-expression functions */
/**************************/
// head
lval_t* builtin_head(lval_t* args) {
    // check for errors
    LASSERT_BOUNDS(args, 1, 1,
        "'head' function called with no arguments",
        "'head' function called with too many arguments");
    LASSERT(args->cell[0]->type == LVAL_QEXPR, args,
        "'head' function needs to be passed a q-expression");
    LASSERT(args->cell[0]->count > 0, args,
        "cannot take the 'head' of an empty list!");

    // take q-expression from arguments
    lval_t* lst = lval_take(args, 0);

    // take the head and return it
    return lval_take(lst, 0);
}

// tail
lval_t* builtin_tail(lval_t* args){
    // check for errors
    LASSERT_BOUNDS(args, 1, 1,
            "'tail' function called with no arguments",
            "'tail' function called with too many arguments");
    LASSERT(args->cell[0]->type == LVAL_QEXPR, args,
            "'tail' function needs to be passed a q-expression");
    LASSERT(args->cell[0]->count > 0, args,
            "cannot take the 'tail' of an empty list!");

    // take q-expression from arguments
    lval_t* lst = lval_take(args, 0);

    // pop head from list and deallocate it
    lval_t* head = lval_pop(lst, 0); lval_del(head);

    // return rest of the list
    return lst;
}

// list
lval_t* builtin_list(lval_t* args) {
    args->type = LVAL_QEXPR;
    return args;
}

// eval
lval_t* lval_eval(lval_t*);
lval_t* builtin_eval(lval_t* args) {
    // check for errors
    LASSERT_BOUNDS(args, 1, 1,
            "'tail' function called with no arguments",
            "'tail' function called with too many arguments");
    LASSERT(args->cell[0]->type == LVAL_QEXPR, args,
            "'tail' function needs to be passed a q-expression");
    LASSERT(args->cell[0]->count > 0, args,
            "cannot take the 'tail' of an empty list!");

    // get q-expression inside arguments
    lval_t* toEval = lval_take(args, 0);

    // evaluate it as if it was an s-expression
    toEval->type = LVAL_SEXPR;
    return lval_eval(toEval);
}

/************************/
/* arithmetic operators */
/************************/
lval_t* builtin_op(const char* op, lval_t* v) {
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

/*******************************************/
/* dispatch symbol to his builtin function */
/*******************************************/
lval_t* builtin(const char* sym, lval_t* args) {
    // dispatch to right builtin function
    if (strcmp(sym, "head") == 0) return builtin_head(args);
    if (strcmp(sym, "tail") == 0) return builtin_tail(args);
    if (strcmp(sym, "list") == 0) return builtin_list(args);
    if (strcmp(sym, "eval") == 0) return builtin_eval(args);
    if (strcmp(sym, "+") == 0 || strcmp(sym, "-") ||
        strcmp(sym, "*") == 0 || strcmp(sym, "/")) return builtin_op(sym, args);

    return lval_err("trying to evaluate unknown builtin!");
}
