#pragma once

#include "core.h"

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

    // process symbol
    else if (strstr(tree->tag, "symbol"))
        return lval_sym(tree->contents);

    // process sexpr OR root of program (treated equally for now)
    else if (strcmp(tree->tag, ">") == 0 ||
             strstr(tree->tag, "expr")) {
        // create sexpr w/ all children of ast
        lval_t* ret = strstr(tree->tag, "qexpr") ?
            lval_qexpr() : lval_sexpr();
        for (int j = 0; j < tree->children_num; ++j) {
            mpc_ast_t* child = tree->children[j];
            // filter useless information
            if (strcmp(child->contents, "(") == 0) continue;
            if (strcmp(child->contents, ")") == 0) continue;
            if (strcmp(child->contents, "{") == 0) continue;
            if (strcmp(child->contents, "}") == 0) continue;
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
