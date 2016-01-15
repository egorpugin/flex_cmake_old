#pragma once

/* Add an accepting state to a machine. */
void add_accept(int, int);

/* Make a given number of copies of a singleton machine. */
int copysingl(int, int);

/* Debugging routine to write out an nfa. */
void dumpnfa(int);

/* Finish up the processing for a rule. */
void finish_rule(int, int, int, int, int);

/* Connect two machines together. */
int link_machines(int, int);

/* Mark each "beginning" state in a machine as being a "normal" (i.e.,
* not trailing context associated) state.
*/
void mark_beginning_as_normal(int);

/* Make a machine that branches to two machines. */
int mkbranch(int, int);

int mkclos(int); /* convert a machine into a closure */
int mkopt(int);  /* make a machine optional */

/* Make a machine that matches either one of two machines. */
int mkor(int, int);

/* Convert a machine into a positive closure. */
int mkposcl(int);

int mkrep(int, int, int); /* make a replicated machine */

/* Create a state with a transition on a given symbol. */
int mkstate(int);

void new_rule(void); /* initialize for a new rule */
