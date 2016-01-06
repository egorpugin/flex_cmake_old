#pragma once

/* Build table entries for dfa state. */
void bldtbl(int[], int, int, int, int);

void cmptmps(void);        /* compress template table entries */
void expand_nxt_chk(void); /* increase nxt/chk arrays */

/* Finds a space in the table for a state to be placed. */
int find_table_space(int *, int);
void inittbl(void); /* initialize transition tables */

/* Make the default, "jam" table entries. */
void mkdeftbl(void);

/* Create table entries for a state (or state fragment) which has
* only one out-transition.
*/
void mk1tbl(int, int, int, int);

/* Place a state into full speed transition table. */
void place_state(int *, int, int);

/* Save states with only one out-transition to be processed later. */
void stack1(int, int, int, int);
