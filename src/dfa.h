#pragma once

/* Check a DFA state for backing up. */
void check_for_backing_up(int, int[]);

/* Increase the maximum number of dfas. */
void increase_max_dfas(void);

void ntod(void); /* convert a ndfa to a dfa */
