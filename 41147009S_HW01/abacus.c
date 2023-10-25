#include "abacus.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sAbacus *abacus_init(void) {
    sAbacus *ret;
    ret = calloc(1, sizeof(sAbacus));
    ret->number = 1;
    ret->pLowerRod = calloc(1, sizeof(uint8_t));
    ret->pUpperRod = calloc(1, sizeof(uint8_t));
    return ret;
}
void abacus_free(sAbacus *this) {
    free(this->pLowerRod);
    free(this->pUpperRod);
    free(this);
    return;
}
int32_t abacus_set(sAbacus *pA, char *pStr) {
    if (pA == NULL) return -1;
    if (pStr == NULL) return -1;
    if (strlen(pStr) > 255) return -1;
    for (int i = 0; pStr[i] != 0x00; ++i) {
        if (pStr[i] > '9' || pStr[i] < '0') return -1;
    }
    if (pA->number < strlen(pStr)) {
        pA->pLowerRod = realloc(pA->pLowerRod, strlen(pStr));
        pA->pUpperRod = realloc(pA->pUpperRod, strlen(pStr));
    }
    pA->number = strlen(pStr);
    for (int i = 0; i < pA->number; ++i) {
        pA->pLowerRod[i] = pStr[i] - '0';
        if (pA->pLowerRod[i] >= 5) {
            pA->pLowerRod[i] -= 5;
            pA->pUpperRod[i] = 1;
        } else
            pA->pUpperRod[i] = 0;
    }
    return 0;
}
int32_t abacus_add(sAbacus *pA, sAbacus b, sAbacus c) {
    if (pA == NULL) return -1;
    uint8_t *tmp;
    int size = ((b.number > c.number) ? b.number : c.number) + 1;
    tmp = calloc(size, sizeof(uint8_t));

    for (int i = b.number - 1; i >= 0; --i) {
        tmp[size - b.number + i] = b.pUpperRod[i] * 5 + b.pLowerRod[i];
    }
    for (int i = c.number - 1; i >= 0; --i) {
        tmp[size - c.number + i] += c.pUpperRod[i] * 5 + c.pLowerRod[i];
        if (tmp[size - c.number + i] >= 10) {
            tmp[size - c.number + i - 1]++;
            tmp[size - c.number + i] -= 10;
        }
    }
    if (tmp[0] && size > 255) {
        free(tmp);
        return -1;
    }
    uint8_t *fre = tmp;
    if (tmp[0] == 0) {
        tmp = &tmp[1];
        size--;
    }
    pA->number = size;
    pA->pLowerRod = realloc(pA->pLowerRod, size);
    pA->pUpperRod = realloc(pA->pUpperRod, size);
    for (int i = 0; i < size; ++i) {
        pA->pLowerRod[i] = tmp[i];
        if (pA->pLowerRod[i] >= 5) {
            pA->pLowerRod[i] -= 5;
            pA->pUpperRod[i] = 1;
        } else
            pA->pUpperRod[i] = 0;
    }
    free(fre);
    return 0;
}
int32_t abacus_del(sAbacus *pA, sAbacus b, sAbacus c) {
    if (pA == NULL) return -1;
    uint8_t *tmp;
    int size = ((b.number > c.number) ? b.number : c.number);
    tmp = calloc(size, sizeof(uint8_t));
    for (int i = b.number - 1; i >= 0; --i) {
        tmp[size - b.number + i] = b.pUpperRod[i] * 5 + b.pLowerRod[i];
    }
    for (int i = c.number - 1; i >= 0; --i) {
        if (tmp[size - c.number + i] < c.pUpperRod[i] * 5 + c.pLowerRod[i]) {
            tmp[size - c.number + i] += 10;
            int j;
            int check = 0;
            for (j = size - c.number + i - 1; j >= 0; --j) {
                if (tmp[j] > 0) {
                    --tmp[j];
                    check = 1;
                    break;
                } else {
                    tmp[j] += 9;
                }
            }
            if (!check) {
                free(tmp);
                return -1;
            }
        }
        tmp[size - c.number + i] -= c.pUpperRod[i] * 5 + c.pLowerRod[i];
    }
    if (tmp[0] && size > 255) {
        free(tmp);
        return -1;
    }
    uint8_t *fre = tmp;
    while (tmp[0] == 0) {
        tmp = &tmp[1];
        size--;
        if (!size) break;
    }
    if (!size) {
        pA->number = 1;
        pA->pLowerRod = realloc(pA->pLowerRod, 1);
        pA->pUpperRod = realloc(pA->pUpperRod, 1);
        pA->pLowerRod[0] = 0;
        pA->pUpperRod[0] = 0;
        free(tmp);
        return 0;
    } else {
        pA->number = size;
        pA->pLowerRod = realloc(pA->pLowerRod, size);
        pA->pUpperRod = realloc(pA->pUpperRod, size);
        for (int i = 0; i < size; ++i) {
            pA->pLowerRod[i] = tmp[i];
            if (pA->pLowerRod[i] >= 5) {
                pA->pLowerRod[i] -= 5;
                pA->pUpperRod[i] = 1;
            } else
                pA->pUpperRod[i] = 0;
        }
    }
    free(fre);
    return 0;
}
int32_t abacus_print(sAbacus a) {
    for (int i = 0; i < a.number; ++i) {
        if (a.pUpperRod[i] > 1 || a.pUpperRod[i] < 0) return -1;
        if (a.pUpperRod[i])
            printf(" ");
        else
            printf("\e[38;5;1m*\e[m");
    }
    printf("\n");
    for (int i = 0; i < a.number; ++i) {
        if (a.pUpperRod[i])
            printf("\e[38;5;1m*\e[m");
        else
            printf(" ");
    }
    printf("\n");
    for (int i = 0; i < a.number; ++i) printf("-");
    printf("\n");
    for (int i = 1; i <= 5; ++i) {
        for (int j = 0; j < a.number; ++j) {
            if (a.pLowerRod[j] > 4 || a.pLowerRod[j] < 0) return -1;
            if (a.pLowerRod[j] == i - 1)
                printf(" ");
            else
                printf("\e[38;5;226m*\e[m");
        }
        printf("\n");
    }
    return 0;
}
