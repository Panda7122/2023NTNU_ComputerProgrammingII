
#include "matrix.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
sMatrix *matrix_init(uint8_t m, uint8_t n) {
    sMatrix *ret;
    if (n && m) {
        ret = calloc(1, sizeof(sMatrix));
        ret->r = m;
        ret->c = n;
        ret->arr = calloc(m, sizeof(int64_t));
        for (int i = 0; i < m; ++i) {
            ret->arr[i] = calloc(n, sizeof(int64_t));
        }
        return ret;
    } else {
        return NULL;
    }
}

int32_t matrix_set(sMatrix *pM, uint8_t m, uint8_t n, int32_t value) {
    // printf("%d %d %d %d\n", m, n, pM->r, pM->c);
    if (pM) {
        if (m >= 0 && n >= 0) {
            if (m >= pM->r || n >= pM->c) {
                return -1;
            }
            pM->arr[m][n] = value;
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

// Print the matrix as follows
// 1 0
// 0 1
void matrix_print(const sMatrix *pM) {
    if (!pM) return;
    for (int i = 0; i < pM->r; ++i) {
        for (int j = 0; j < pM->c; ++j) {
            printf("%ld ", pM->arr[i][j]);
        }
        printf("\n");
    }
}

// A = B + C
// If error , return -1; otherwise , return 0;
int32_t matrix_add(sMatrix *pA, const sMatrix *pB, const sMatrix *pC) {
    if (!pB || !pC || !pA) return -1;
    if (pB->r != pC->r) return -1;
    if (pB->c != pC->c) return -1;
    for (int i = 0; i < pA->r; ++i) free(pA->arr[i]);
    free(pA->arr);
    pA->arr = calloc(pB->r, sizeof(int64_t *));

    for (int i = 0; i < pB->r; ++i) {
        pA->arr[i] = calloc(pB->c, sizeof(int64_t));
    }
    pA->r = pB->r;
    pA->c = pB->c;
    for (int i = 0; i < pA->r; ++i) {
        for (int j = 0; j < pA->c; ++j) {
            pA->arr[i][j] = pB->arr[i][j] + pC->arr[i][j];
        }
    }
    return 0;
}

// A = B * C
// If error , return -1; otherwise , return 0;
int32_t matrix_multiply(sMatrix *pA, const sMatrix *pB, const sMatrix *pC) {
    if (!pB || !pC || !pA) return -1;
    if (pB->c != pC->r) return -1;
    // printf("realloc\n");
    for (int i = 0; i < pA->r; ++i) free(pA->arr[i]);
    free(pA->arr);
    pA->arr = calloc(pB->r, sizeof(int64_t *));

    for (int i = 0; i < pB->r; ++i) {
        pA->arr[i] = calloc(pC->c, sizeof(int64_t));
    }
    pA->r = pB->r;
    pA->c = pC->c;
    // printf("reset\n");
    for (int i = 0; i < pA->r; ++i) {
        for (int j = 0; j < pA->c; ++j) pA->arr[i][j] = 0;
    }
    // printf("start\n");
    for (int i = 0; i < pB->r; ++i) {
        for (int j = 0; j < pC->c; ++j) {
            for (int k = 0; k < pC->r; ++k) {
                // printf("A[%d][%d] += B[%d][%d](%ld) * C[%d][%d](%ld)\n", i,
                // j,
                //    i, k, pB->arr[i][k], k, j, pC->arr[k][j]);
                pA->arr[i][j] += pB->arr[i][k] * pC->arr[k][j];
            }
        }
    }
    return 0;
}

// A = A'
// If error , return -1; otherwise , return 0;
int32_t matrix_transpose(sMatrix *pA) {
    if (!pA) return -1;
    int r, c;
    int **arr;
    r = pA->r;
    c = pA->c;
    arr = calloc(r, sizeof(int64_t));
    for (int i = 0; i < r; ++i) {
        arr[i] = calloc(c, sizeof(int64_t));
    }
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            arr[i][j] = pA->arr[i][j];
        }
    }
    for (int i = 0; i < pA->r; ++i) free(pA->arr[i]);
    free(pA->arr);
    pA->arr = calloc(pA->c, sizeof(int64_t *));
    for (int i = 0; i < pA->c; ++i) {
        pA->arr[i] = calloc(pA->r, sizeof(int64_t));
    }
    pA->r = c;
    pA->c = r;
    for (int i = 0; i < c; ++i) {
        for (int j = 0; j < r; ++j) {
            pA->arr[i][j] = arr[j][i];
        }
    }
    for (int i = 0; i < r; ++i) {
        free(arr[i]);
    }
    free(arr);
    return 0;
}

// Determinant
// Determinant should be put to *pAns
// If error , return -1; otherwise , return 0;
static int32_t func(const sMatrix *pA) {
    if (pA->c == 1) return pA->arr[0][0];
    int ret = 0;
    for (int i = 0; i < pA->r; ++i) {
        sMatrix *tmp;
        tmp = matrix_init(pA->r - 1, pA->c - 1);
        for (int j = 0; j < pA->r; ++j) {
            if (j == i) continue;
            for (int k = 1; k < pA->c; ++k) {
                matrix_set(tmp, j - (j > i), k - 1, pA->arr[j][k]);
            }
        }
        if (pA->arr[i][0])
            ret += func(tmp) * pA->arr[i][0] * ((i & 1) ? -1 : 1);
        matrix_free(tmp);
    }
    return ret;
}
int32_t matrix_det(const sMatrix *pA, int32_t *pAns) {
    if (!pA || !pAns) return -1;
    if (pA->r != pA->c) return -1;

    *pAns = func(pA);
    return 0;
}

// Free
// If error , return -1; otherwise , return 0;
int32_t matrix_free(sMatrix *pA) {
    if (!pA) return -1;
    if (!pA->arr) return -1;
    for (int i = 0; i < pA->r; ++i) free(pA->arr[i]);
    free(pA->arr);
    free(pA);
    return 0;
}