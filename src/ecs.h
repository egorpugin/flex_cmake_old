#pragma once

/* Convert character classes to set of equivalence classes. */
void ccl2ecl(void);

/* Associate equivalence class numbers with class members. */
int cre8ecs(int[], int[], int);

/* Update equivalence classes based on character class transitions. */
void mkeccl(unsigned char[], int, int[], int[], int, int);

/* Create equivalence class for single character. */
void mkechar(int, int[], int[]);
