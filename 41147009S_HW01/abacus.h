#ifndef _ABACUS_H
#define _ABACUS_H
#include <stdint.h>

typedef struct _sAbacus {
    uint8_t number;      // 0-255
    uint8_t *pUpperRod;  // Each element is in {0,1}
    uint8_t *pLowerRod;  // Each element is in {0,1,2,3,4}
} sAbacus;

sAbacus *abacus_init(void);
void abacus_free(sAbacus *);

// Set pA according to pStr
// For example , pStr is "123456789"
// pA -> number = 9;
// pA -> pUpperRod: {0,0,0,0,1,1,1,1,1}
// pA -> pLowerRod: {1,2,3,4,0,1,2,3,4}
// if the length of pStr > 255 or the string contains alphabets or
// specialcharacter or NULL, return -1. Otherwise , return 0
int32_t abacus_set(sAbacus *pA, char *pStr);

// a = b + c
// Successful: return 0; otherwise , return -1
int32_t abacus_add(sAbacus *pA, sAbacus b, sAbacus c);

// a = b - c
// Successful: return 0; otherwise , return -1
int32_t abacus_del(sAbacus *pA, sAbacus b, sAbacus c);

// EX: "172", you should print
// * *
//  *
// ---
// ***
//  **
// *
// ***
// ***
// Successful: return 0; otherwise , return -1
int32_t abacus_print(sAbacus a);
#endif