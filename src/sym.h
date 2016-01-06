#pragma once

#include "common.h"

/* Save the text of a character class. */
void cclinstal(const String &, int);

/* Lookup the number associated with character class. */
int ccllookup(const String &);

/* Increase maximum number of SC's. */
void scextend();
void scinstal(const String &, int); /* make a start condition */

/* Lookup the number associated with a start condition. */
int sclookup(const String &);
