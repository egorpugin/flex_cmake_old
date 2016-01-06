#pragma once

#include "common.h"

/* Increase maximum number of SC's. */
void scextend();
void scinstal(const String &, int); /* make a start condition */

/* Lookup the number associated with a start condition. */
int sclookup(const String &);
