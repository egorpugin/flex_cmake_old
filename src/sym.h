#pragma once

/* Save the text of a character class. */
void cclinstal(unsigned char*, int);

/* Lookup the number associated with character class. */
int ccllookup(unsigned char*);

void ndinstal(char*, unsigned char[]); /* install a name definition */
unsigned char* ndlookup(const char*);        /* lookup a name definition */

/* Increase maximum number of SC's. */
void scextend(void);
void scinstal(const char*, int); /* make a start condition */

/* Lookup the number associated with a start condition. */
int sclookup(const char*);
