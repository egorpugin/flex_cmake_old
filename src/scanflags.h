#pragma once

#include <stdint.h>

typedef unsigned int scanflags_t;
extern scanflags_t *_sf_stk;
extern size_t _sf_top_ix, _sf_max; /**< stack of scanner flags. */

#define _SF_CASE_INS 0x0001
#define _SF_DOT_ALL 0x0002
#define _SF_SKIP_WS 0x0004
#define sf_top() (_sf_stk[_sf_top_ix])
#define sf_case_ins() (sf_top() & _SF_CASE_INS)
#define sf_dot_all() (sf_top() & _SF_DOT_ALL)
#define sf_skip_ws() (sf_top() & _SF_SKIP_WS)
#define sf_set_case_ins(X) ((X) ? (sf_top() |= _SF_CASE_INS) : (sf_top() &= ~_SF_CASE_INS))
#define sf_set_dot_all(X) ((X) ? (sf_top() |= _SF_DOT_ALL) : (sf_top() &= ~_SF_DOT_ALL))
#define sf_set_skip_ws(X) ((X) ? (sf_top() |= _SF_SKIP_WS) : (sf_top() &= ~_SF_SKIP_WS))

extern void sf_init(void);
extern void sf_push(void);
extern void sf_pop(void);
