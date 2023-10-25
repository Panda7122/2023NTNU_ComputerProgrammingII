#include "insert.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int32_t strinsert(char **ppResult, const char *pStr1, int32_t location,
                  const char *pStr2) {
    if (location < 0) return -1;
    int32_t cnt = 0;
    *ppResult = calloc(strlen(pStr1) + strlen(pStr2) + 1, sizeof(char));
    for (int i = 0; i < location; ++i) {
        (*ppResult)[cnt] = pStr1[i];
        ++cnt;
        if (pStr1[i] == 0x00) return -1;
    }
    for (int i = 0; pStr2[i] != 0x00; ++i) {
        (*ppResult)[cnt] = pStr2[i];
        ++cnt;
        // if(pStr2[i] == 0x00) return 1;
    }
    for (int i = location; pStr1[i] != 0x00; ++i) {
        (*ppResult)[cnt] = pStr1[i];
        ++cnt;
    }
    (*ppResult)[cnt] = 0x00;
    return 0;
}