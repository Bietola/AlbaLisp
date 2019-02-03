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
lval_t* builtin_head(lval_t* args) {
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
lval_t* builtin_tail(lval_t* args){
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
lval_t* builtin_list(lval_t* args) {
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
lval_t* builtin(const char* sym, env_t* env, lval_t* args) {
    // dispatch to right builtin function
    if (strcmp(sym, "head") == 0) return builtin_head(args);
    if (strcmp(sym, "tail") == 0) return builtin_tail(args);
    if (strcmp(sym, "list") == 0) return builtin_list(args);
    if (strcmp(sym, "eval") == 0) return builtin_eval(env, args);
    if (strcmp(sym, "def")  == 0) return builtin_def (env, args);
    if (strcmp(sym, "+") == 0 || strcmp(sym, "-") ||
        strcmp(sym, "*") == 0 || strcmp(sym, "/")) return builtin_op(sym, args);

    return lval_err("trying to evaluate unknown builtin!");
}
