#pragma once

#include "common.h"

/* Save the text of a character class. */
void cclinstal(const String &, int);

/* Lookup the number associated with character class. */
int ccllookup(const String &);

void ndinstal(const String &, unsigned char[]); /* install a name definition */
unsigned char *ndlookup(const String &);  /* lookup a name definition */

/* Increase maximum number of SC's. */
void scextend(void);
void scinstal(const String &, int); /* make a start condition */

/* Lookup the number associated with a start condition. */
int sclookup(const String &);
