#ifndef _MACROS_H
#define _MACROS_H
/* Bits related Macros */

// Return 2 to x. You do not need to worry the overflow issue.
#define BIT(x) (1 << x)

// Set x's p-th bit to 1. Count from 0 and from LSB.
#define SETBIT(x, p) (x |= BIT(p))
// Set x's p-th bit to 0. Count from 0 and from LSB.
#define CLEARBIT(x, p) (x &= ((~0) ^ BIT(p)))
// Get x's p-th bit to 1. Count from 0 and from LSB.
#define GETBIT(x, p) ((x & BIT(p)) >> p)
// Toggle x's p-th bit to 1. Count from 0 and from LSB.
#define TOGGLEBIT(x, p) (x ^= BIT(p))
/* Loop related Macros */
// Example: RANGE (i, 10, 20)
// it will be 10,11,12,13,14,15,16,17,18,19,20
// Example: RANGE (i, 5, -5)
// it will be 5,4,3,2,1,0,-1,-2,-3,-4,-5
#define RANGE(i, y, x) \
    for (i = y; (y < x ? i <= x : i >= x); (y < x ? ++i : --i))

// FOREACH (item, array)
// A C implementation of foreach loop, it will go through each element of an
// array , and will perform operations on each element as returned into
// pointer item variable.
#define FOREACH(i, A) for (i = A; ((int8_t*)i - (int8_t*)A) < sizeof(A); ++i)
#endif