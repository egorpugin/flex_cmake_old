#pragma once

#include <stdio.h>

void ccladd(int, int);           /* add a single character to a ccl */
int cclinit(void);               /* make an empty ccl */
void cclnegate(int);             /* negate a ccl */
int ccl_set_diff(int a, int b);  /* set difference of two ccls. */
int ccl_set_union(int a, int b); /* set union of two ccls. */

/* List the members of a set of characters in CCL form. */
void list_character_set(FILE *, int[]);
