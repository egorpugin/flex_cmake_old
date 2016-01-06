#pragma once

/* Check a DFA state for backing up. */
void check_for_backing_up(int, int[]);

/* Check to see if NFA state set constitutes "dangerous" trailing context. */
void check_trailing_context(int *, int, int *, int);

/* Construct the epsilon closure of a set of ndfa states. */
int *epsclosure(int *, int *, int[], int *, int *);

/* Increase the maximum number of dfas. */
void increase_max_dfas(void);

void ntod(void); /* convert a ndfa to a dfa */

/* Converts a set of ndfa states into a dfa state. */
int snstods(int[], int, int[], int, int, int *);
