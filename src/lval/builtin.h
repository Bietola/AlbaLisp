#pragma once

#include "core.h"
#include "lassert.h"

/**********************************************************/
/*          builtin operators and functions               */
/*--------------------------------------------------------*/
/* NB: all builtin functions expect an s-expression       */
/*     as their sole parameter. This expression countains */
/*     every piece of input that the operator requires.   */
/**********************************************************/

// forward declarations
lval_t* lval_eval(env_t*, lval_t*);

/**************************/
/* environment primitives */
/**************************/

// def
lval_t* builtin_def(env_t* env, lval_t* args) {
    // 1st arg: one q-epxression with list of variables
    // from 2nd arg on: values to assign to variables
    LASSERT_AT_LEAST(args, 2, "def");
    LASSERT_TYPES(args, LVAL_QEXPR, "def");

    // get variables list
    lval_t* vars = lval_pop(args, 0);

    // ensure that all variables are symbols
    for (int j = 0; j < vars->count; ++j) {
        if (vars->cell[j]->type != LVAL_SYM) {
            lval_del(args);
            return lval_err("only symbols may be used as variables.");
        }
    }

    // associate variables to their values and add
    // them to the environment
    while (vars->count) {
        lval_t* sym = lval_pop(vars, 0);
        lval_t* val = lval_pop(args, 0);
        if (val)
            env_add(env, sym, lval_eval(env, val));
        else
            // assign nil to var if no more values to
            // bind are available in args
            env_add(env, sym, lval_nil());
    }

    // cleanup
    lval_del(vars); lval_del(args);

    // always return nil
    return lval_nil();
}

/**************************/
/* q-expression functions */
/**************************/
// head
lval_t* builtin_head(env_t* env, lval_t* args) {
    // check for errors
    LASSERT_BOUNDS(args, 1, 1, "head");
    LASSERT_TYPES(args, LVAL_QEXPR, "head");
    LASSERT(args->cell[0]->count > 0, args,
        "cannot take the 'head' of an empty list!");

    // take q-expression from arguments
    lval_t* lst = lval_take(args, 0);

    // take the head and return it
    return lval_take(lst, 0);
}

// tail
lval_t* builtin_tail(env_t* env, lval_t* args){
    // check for errors
    LASSERT_BOUNDS(args, 1, 1, "tail");
    LASSERT_TYPES(args, LVAL_QEXPR, "tail");
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
lval_t* builtin_list(env_t* env, lval_t* args) {
    args->type = LVAL_QEXPR;
    return args;
}

// eval
lval_t* builtin_eval(env_t* env, lval_t* args) {
    // check for errors
    LASSERT_BOUNDS(args, 1, 1, "eval");
    LASSERT_TYPES(args, LVAL_QEXPR, "eval");
    LASSERT(args->cell[0]->count > 0, args,
            "cannot evaluate empty list!");

    // get q-expression inside arguments
    lval_t* toEval = lval_take(args, 0);

    // evaluate it as if it was an s-expression
    toEval->type = LVAL_SEXPR;
    return lval_eval(env, toEval);
}

/************************/
/* arithmetic operators */
/************************/
// real work
lval_t* builtin_op(const char* op, lval_t* args) {
    // ensure all arguments are numbers
    for (int j = 0; j < args->count; ++j) {
        if (args->cell[j]->type != LVAL_NUM) {
            lval_del(args);
            return lval_err("cannot operate on non-number!");
        }
    }

    // take first element
    lval_t* acc = lval_pop(args, 0);

    // unary minus
    if (args->count == 0 && strcmp(op, "-") == 0) {
        acc->num = -acc->num;
        lval_del(args);
        return acc;
    }

    // more than one element
    while (args->count) {
        lval_t* numv = lval_pop(args, 0);

        // match operator
        if      (strcmp(op, "+") == 0) acc->num += numv->num;
        else if (strcmp(op, "-") == 0) acc->num -= numv->num;
        else if (strcmp(op, "*") == 0) acc->num *= numv->num;
        else if (strcmp(op, "/") == 0) {
            if (numv->num == 0) {
                lval_del(acc); lval_del(numv);
                acc = lval_err("cannot perform division by 0!");
                break;
            }
            else
                acc->num /= numv->num;
        }
    }

    lval_del(args); return acc;
}

// dispatchers
// add
lval_t* builtin_add(env_t* env, lval_t* args) {
    return builtin_op("+", args);
}
// subtract
lval_t* builtin_subtract(env_t* env, lval_t* args) {
    return builtin_op("-", args);
}
// multiply
lval_t* builtin_multiply(env_t* env, lval_t* args) {
    return builtin_op("*", args);
}
// divide
lval_t* builtin_divide(env_t* env, lval_t* args) {
    return builtin_op("/", args);
}
