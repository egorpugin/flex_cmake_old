#pragma once

/* Generate the code to keep backing-up information. */
void gen_backing_up(void);

/* Generate the code to perform the backing up. */
void gen_bu_action(void);

/* Generate full speed compressed transition table. */
void genctbl(void);

/* Generate the code to find the action number. */
void gen_find_action(void);

void genftbl(void); /* generate full transition table */

/* Generate the code to find the next compressed-table state. */
void gen_next_compressed_state(char *);

/* Generate the code to find the next match. */
void gen_next_match(void);

/* Generate the code to find the next state. */
void gen_next_state(int);

/* Generate the code to make a NUL transition. */
void gen_NUL_trans(void);

/* Generate the code to find the start state. */
void gen_start_state(void);

/* Generate data statements for the transition tables. */
void gentabs(void);

void make_tables(void); /* generate transition tables */
