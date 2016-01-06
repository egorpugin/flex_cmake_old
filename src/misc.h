#pragma once

/* Add a #define to the action file. */
void action_define(const char *defname, int value);

/* Add the given text to the stored actions. */
void add_action(const char *new_text);

/* True if a string is all lower case. */
int all_lower(char *);

/* True if a string is all upper case. */
int all_upper(char *);

/* Compare two integers for use by qsort. */
int intcmp(const void *, const void *);

/* Check a character to make sure it's in the expected range. */
void check_char(int c);

/* Replace upper-case letter to lower-case. */
unsigned char clower(int);

/* strdup() that fails fatally on allocation failures. */
char *xstrdup(const char *);

/* Compare two characters for use by qsort with '\0' sorting last. */
int cclcmp(const void *, const void *);

/* Finish up a block of data declarations. */
void dataend(void);

/* Flush generated data statements. */
void dataflush(void);

/* Report an error message and terminate. */
void flexerror(const char *);

/* Report a fatal error message and terminate. */
void flexfatal(const char *);

/* Report a fatal error with a pinpoint, and terminate */
#if HAVE_DECL___FUNC__
#define flex_die(msg)                                              \
    do                                                             \
    {                                                              \
        fprintf(stderr,                                            \
                _("%s: fatal internal error at %s:%d (%s): %s\n"), \
                program_name, __FILE__, (int)__LINE__,             \
                __func__, msg);                                    \
        FLEX_EXIT(1);                                              \
    } while (0)
#else /* ! HAVE_DECL___FUNC__ */
#define flex_die(msg)                                        \
    do                                                       \
    {                                                        \
        fprintf(stderr,                                      \
                _("%s: fatal internal error at %s:%d %s\n"), \
                program_name, __FILE__, (int)__LINE__,       \
                msg);                                        \
        FLEX_EXIT(1);                                        \
    } while (0)
#endif /* ! HAVE_DECL___func__ */

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

/* Generate a data statment for a two-dimensional array. */
void mk2data(int);

void mkdata(int); /* generate a data statement */

/* Return character corresponding to escape sequence. */
unsigned char myesc(unsigned char[]);

/* Return a printable version of the given character, which might be
* 8-bit.
*/
char *readable_form(int);

/* Write out one section of the skeleton file. */
void skelout(void);

/* Output a yy_trans_info structure. */
void transition_struct_out(int, int);
