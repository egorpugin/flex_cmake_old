/* sym - symbol table routines */

/*  Copyright (c) 1990 The Regents of the University of California. */
/*  All rights reserved. */

/*  This code is derived from software contributed to Berkeley by */
/*  Vern Paxson. */

/*  The United States Government has rights in this work pursuant */
/*  to contract no. DE-AC03-76SF00098 between the United States */
/*  Department of Energy and the University of California. */

/*  This file is part of flex. */

/*  Redistribution and use in source and binary forms, with or without */
/*  modification, are permitted provided that the following conditions */
/*  are met: */

/*  1. Redistributions of source code must retain the above copyright */
/*     notice, this list of conditions and the following disclaimer. */
/*  2. Redistributions in binary form must reproduce the above copyright */
/*     notice, this list of conditions and the following disclaimer in the */
/*     documentation and/or other materials provided with the distribution. */

/*  Neither the name of the University nor the names of its contributors */
/*  may be used to endorse or promote products derived from this software */
/*  without specific prior written permission. */

/*  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR */
/*  PURPOSE. */

#include <unordered_map>

#include "sym.h"

#include "flexdef.h"

#include "misc.h"
#include "nfa.h"

/* Variables for symbol tables:
 * sctbl - start-condition symbol table
 * ndtbl - name-definition symbol table
 * ccltab - character class text symbol table
 */

std::unordered_map<String, String> ndtbl;
std::unordered_map<String, int> sctbl;
std::unordered_map<String, int> ccltab;

/* cclinstal - save the text of a character class */
void cclinstal(unsigned char* ccltxt, int cclnum)
{
    /* We don't bother checking the return status because we are not
	 * called unless the symbol is new.
	 */
    ccltab[(const char*)ccltxt] = cclnum;
}

/* ccllookup - lookup the number associated with character class text
 *
 * Returns 0 if there's no CCL associated with the text.
 */
int ccllookup(unsigned char* ccltxt)
{
    auto i = ccltab.find((const char*)ccltxt);
    if (i == ccltab.end())
        return 0;
    return i->second;
}

/* ndinstal - install a name definition */
void ndinstal(char *name, unsigned char* definition)
{
    auto i = ndtbl.find(name);
    if (i != ndtbl.end())
        synerr(_("name defined twice"));
    ndtbl[name] = (const char*)definition;
}

/* ndlookup - lookup a name definition
 *
 * Returns a nil pointer if the name definition does not exist.
 */
unsigned char* ndlookup(const char *nd)
{
    auto i = ndtbl.find(nd);
    if (i == ndtbl.end())
        return 0;
    return (unsigned char*)i->second.c_str();
}

/* scextend - increase the maximum number of start conditions */
void scextend(void)
{
    current_max_scs += MAX_SCS_INCREMENT;

    ++num_reallocs;

    scset = (decltype(scset))reallocate_integer_array(scset, current_max_scs);
    scbol = (decltype(scbol))reallocate_integer_array(scbol, current_max_scs);
    scxclu = (decltype(scxclu))reallocate_integer_array(scxclu, current_max_scs);
    sceof = (decltype(sceof))reallocate_integer_array(sceof, current_max_scs);
    scname = (decltype(scname))reallocate_char_ptr_array(scname, current_max_scs);
}

/* scinstal - make a start condition
 *
 * NOTE
 *    The start condition is "exclusive" if xcluflg is true.
 */
void scinstal(const char *str, int xcluflg)
{
    if (++lastsc >= current_max_scs)
        scextend();

    scname[lastsc] = xstrdup(str);

    auto i = sctbl.find(str);
    if (i != sctbl.end())
        format_pinpoint_message(_("start condition %s declared twice"), str);
    sctbl[scname[lastsc]] = lastsc;

    scset[lastsc] = mkstate(SYM_EPSILON);
    scbol[lastsc] = mkstate(SYM_EPSILON);
    scxclu[lastsc] = xcluflg;
    sceof[lastsc] = false;
}

/* sclookup - lookup the number associated with a start condition
 *
 * Returns 0 if no such start condition.
 */
int sclookup(const char *str)
{
    auto i = sctbl.find(str);
    if (i == sctbl.end())
        return 0;
    return i->second;
}
