#include "mystr.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int mystrbracket(char ***pppList, int *pCounter, const char *pStr,
                 const char left_bracket, const char right_bracket) {
    if (left_bracket == 0 || right_bracket == 0 || pStr == NULL ||
        pCounter == NULL)
        return -1;
    *pCounter = 0;
    *pppList = calloc(1, sizeof(char *));
    for (int i = 0; pStr[i] != 0x00; ++i) {
        if (pStr[i] == left_bracket) {
            int j = i;
            int counter = 1;
            while (counter && pStr[j] != 0x00) {
                ++j;
                if (pStr[j] == left_bracket)
                    ++counter;
                else if (pStr[j] == right_bracket)
                    --counter;
            }
            if (counter) {
                return -1;
            }
            (*pCounter)++;
            *pppList = realloc(*pppList, *pCounter * sizeof(char *));
            (*pppList)[(*pCounter) - 1] = calloc(j - i, sizeof(char));
            strncpy((*pppList)[(*pCounter) - 1], pStr + i + 1, j - i - 1);
        }
    }
    return 0;
}
