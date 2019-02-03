#pragma once

// assertion macro used to return error msg
#define LASSERT(CND, LVL, ERRMSG) \
    if (!(CND)) {                   \
        lval_del(LVL);            \
        return lval_err(ERRMSG);  \
    }

// functions bounds assertion
#define LASSERT_AT_LEAST(LVL, LIM, FNAME) \
    LASSERT(LVL->count >= LIM, LVL, \
        "'" #FNAME "' function called with too few arguments")
#define LASSERT_AT_MOST(LVL, LIM, FNAME) \
    LASSERT(LVL->count <= LIM, LVL, \
        "'" #FNAME "' function called with too many arguments")
#define LASSERT_BOUNDS(LVL, MINB, MAXB, FNAME) \
    LASSERT_AT_LEAST(LVL, MINB, FNAME) \
    LASSERT_AT_MOST (LVL, MAXB, FNAME)

// type of args
#define LASSERT_TYPES(LVL, TP1, FNAME) \
    LASSERT(LVL->cell[0]->type == TP1, LVL, \
        "'" #FNAME "' needs to be passed a 1st argument of type '" #TP1 "'")
