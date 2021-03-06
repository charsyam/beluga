/*
 *  constant-folding and optimization
 */

#ifndef SIMP_H
#define SIMP_H

#include "tree.h"
#include "ty.h"


extern int simp_needconst;    /* > 0 while a constant is necessary */


tree_t *simp_intexpr(int, long *, int, const char *);
tree_t *simp_basetree(const sym_t *, tree_t *);
tree_t *simp_tree_s(int, ty_t *, tree_t *, tree_t *);
tree_t *simp_cvtree_s(int, ty_t *, ty_t *, tree_t *);


#endif    /* SIMP_H */

/* end of simp.h */
