#pragma once

#include "common.h"

/* Add a #define to the action file. */
void action_define(const String &defname, int value);

/* Add the given text to the stored actions. */
void add_action(const String &new_text);

/* True if a string is all lower case. */
bool is_all_lower(const String &);

/* True if a string is all upper case. */
bool is_all_upper(const String &);

/* Compare two integers for use by qsort. */
int intcmp(const void *, const void *);

/* Check a character to make sure it's in the expected range. */
void check_char(int c);

/* Replace upper-case letter to lower-case. */
unsigned char clower(int);

/* strdup() that fails fatally on allocation failures. */
char *xstrdup(const char *);

/* Finish up a block of data declarations. */
void dataend();

/* Flush generated data statements. */
void dataflush();

/* Report an error message and terminate. */
void flexerror(const String &);

/* Report a fatal error message and terminate. */
void flexfatal(const String &);

/* Report a fatal error with a pinpoint, and terminate */
void report_internal(const String &s, const char *file, int line, const char *func);

#define flex_die(msg) report_internal(msg, __FILE__, __LINE__, __func__)

/* Report an error message formatted  */
void lerr(const char *, ...)
#if defined(__GNUC__) && __GNUC__ >= 3
    __attribute__((__format__(__printf__, 1, 2)))
#endif
    ;

/* Like lerr, but also exit after displaying message. */
void lerr_fatal(const char *, ...)
#if defined(__GNUC__) && __GNUC__ >= 3
    __attribute__((__format__(__printf__, 1, 2)))
#endif
    ;

/* Spit out a "#line" statement. */
void line_directive_out(bool print, bool infile);

/* Mark the current position in the action array as the end of the section 1
* user defs.
*/
void mark_defs1(void);

/* Mark the current position in the action array as the end of the prolog. */
void mark_prolog(void);

void mkdata(int); /* generate a data statement */

/* Return character corresponding to escape sequence. */
unsigned char myesc(unsigned char[]);

/* Return a printable version of the given character, which might be
* 8-bit.
*/
const char *readable_form(int);

/* Write out one section of the skeleton file. */
void skelout(void);

/* Output a yy_trans_info structure. */
void transition_struct_out(int, int);
